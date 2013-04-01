#ifndef _GL_OPENBOX_H_
#define _GL_OPENBOX_H_

#include <SceneGraph/GLObject.h>
#include <SceneGraph/GLColor.h>

namespace SceneGraph
{

/////////////////////////////////////////////////////////////////////////////
class GLOpenBox : public GLObject
{
    public:
        GLOpenBox(double interval = 1.0, int sub_ticks_per_interval = 10)
            : GLObject("GLBox"),
              m_box_grow_factor(1,1,1),
              m_interval(interval),
              m_ticks_per_interval(sub_ticks_per_interval),
              m_ambient(1.4, 1.4, 1.4, 1.0 ),
              m_diffuse(0.63, 0.65, 0.67, 1.0),
              m_linemod(0.686, 0.874, 0.929, 1.0),
              m_specular(0,0,0,1)
        {
            m_lambient = m_ambient.array() * m_linemod;
            m_ldiffuse = m_diffuse.array() * m_linemod;
        }
        
        inline void DrawFace(const Eigen::Vector3d& bmin, const Eigen::Vector3d& bmax, int axis, double nmag)
        {
            Eigen::Vector3f n(0,0,0);
            Eigen::Vector3f v;
            
            n(axis) = nmag;
            v(axis) = (nmag > 0) ? bmin(axis) : bmax(axis);

            glNormal3fv(n.data());
            
            int o1 = (axis+1)%3;
            int o2 = (axis+2)%3;
            
            if(nmag > 0) std::swap(o1,o2);
                        
            glBegin(GL_QUADS);            
            v(o1) = bmin(o1); v(o2) = bmax(o2); glVertex3fv(v.data());
            v(o1) = bmax(o1); v(o2) = bmax(o2); glVertex3fv(v.data());
            v(o1) = bmax(o1); v(o2) = bmin(o2); glVertex3fv(v.data());
            v(o1) = bmin(o1); v(o2) = bmin(o2); glVertex3fv(v.data());
            glEnd();
        }

        inline void DrawFaceLines(const Eigen::Vector3d& bmin, const Eigen::Vector3d& bmax, int axis, double nmag)
        {
            Eigen::Vector3f n(0,0,0);
            Eigen::Vector3f v;
            
            n(axis) = nmag;
            v(axis) = (nmag > 0) ? bmin(axis) : bmax(axis);
            
            int o1 = (axis+1)%3;
            int o2 = (axis+2)%3;
            
            if(nmag > 0) std::swap(o1,o2);
            
            glNormal3fv(n.data());
            
            float origLineWidth;
            glGetFloatv(GL_LINE_WIDTH, &origLineWidth);
            
            for(float intrvl=m_interval, ls=1.0; intrvl >= 0.1; intrvl /= m_ticks_per_interval, ls /= 2)
            {
                glLineWidth(origLineWidth*ls);
                glBegin(GL_LINES);
                for(float xv= intrvl*ceil(bmin(o1) / intrvl); xv < bmax(o1); xv += intrvl)
                {
                    v(o1) = xv;
                    v(o2) = bmin(o2); glVertex3fv(v.data());
                    v(o2) = bmax(o2); glVertex3fv(v.data());
                }
                for(float xv= intrvl*ceil(bmin(o2) / intrvl); xv < bmax(o2); xv += intrvl)
                {
                    v(o2) = xv;
                    v(o1) = bmin(o1); glVertex3fv(v.data());
                    v(o1) = bmax(o1); glVertex3fv(v.data());
                }
                glEnd();
            }
            glLineWidth(origLineWidth);
        }
        
        inline void DrawOpenBox(const AxisAlignedBoundingBox& bbox)
        {
            glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_LINE_BIT);
            {
                // Prevent Z-Fighting between plane and lines
                glPolygonOffset( 1.0, 1.0 );
                glEnable(GL_POLYGON_OFFSET_FILL);
                
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
                
                const Eigen::Vector3d bmin = bbox.Min();
                const Eigen::Vector3d bmax = bbox.Max();

                // TODO: use projectmatrix too to cope with different cameras                
                Eigen::Matrix4f T_co;
                glGetFloatv( GL_MODELVIEW_MATRIX, T_co.data() );
            
                glDisable(GL_COLOR_MATERIAL);
                glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, m_ambient.data() );
                glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, m_diffuse.data() );
                glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, m_specular.data() );
                
                for(int ax=0; ax<3; ++ax) {
                    Eigen::Vector3f n(0,0,0); n(ax) = 1;
                    Eigen::Vector3f nc = T_co.topLeftCorner<3,3>() * n;
                    DrawFace(bmin,bmax,ax,nc(2)>0 ? 1 : -1);
                }

                glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
                glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, m_lambient.data() );
                glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, m_ldiffuse.data() );

                for(int ax=0; ax<3; ++ax) {
                    Eigen::Vector3f n(0,0,0); n(ax) = 1;
                    Eigen::Vector3f nc = T_co.topLeftCorner<3,3>() * n;
                    DrawFaceLines(bmin,bmax,ax,nc(2)>0 ? 1 : -1);
                }
            }
            glPopAttrib();
        }
        
        Eigen::Vector3d& SizeFactor()
        {
            return m_box_grow_factor;
        }

        void DrawCanonicalObject(void)
        {
            AxisAlignedBoundingBox bbox = ChildrenBounds();
            if(!bbox.Empty()) {
                bbox.ScaleFromCenter(m_box_grow_factor);
                GLObject::m_aabb = bbox;
            }else{
                GLObject::m_aabb = AxisAlignedBoundingBox(Eigen::Vector3d(-1,-1,-1), Eigen::Vector3d(1,1,1) );
            }
            DrawOpenBox(GLObject::m_aabb);
        }

protected:
        Eigen::Vector3d m_box_grow_factor;
        double m_interval;
        int m_ticks_per_interval;
        
        Eigen::Vector4f m_ambient;
        Eigen::Vector4f m_diffuse;
        Eigen::Array4f  m_linemod;
        Eigen::Vector4f m_lambient;
        Eigen::Vector4f m_ldiffuse;
        Eigen::Vector4f m_specular;
};

} // SceneGraph

#endif

