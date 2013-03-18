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
              m_colorPlane(0.4f,0.4f,0.4f,1.0f),
              m_colorLines(0.5f,0.5f,0.5f,1.0)
        {
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
            
            for(float intrvl=m_interval, lw=2.0; intrvl >= 0.1; intrvl /= m_ticks_per_interval, lw /= 2)
            {
                glLineWidth(lw);
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
        }
        
        inline void DrawOpenBox(const AxisAlignedBoundingBox& bbox, GLColor colorPlane, GLColor colorLines)
        {
            glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
            {
                // Prevent Z-Fighting between plane and lines
                glPolygonOffset( 1.0, 1.0 );
                glEnable(GL_POLYGON_OFFSET_FILL);
                
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
                
                const Eigen::Vector3d bmin = bbox.Min();
                const Eigen::Vector3d bmax = bbox.Max();
                
                Eigen::Matrix4f T_co;
                Eigen::Matrix4f P;
                glGetFloatv( GL_MODELVIEW_MATRIX, T_co.data() );
                glGetFloatv( GL_PROJECTION_MATRIX, P.data() );
            
                glDisable(GL_COLOR_MATERIAL);
//                glColor3ub(243, 247, 248);
//                glColor3f(0.953, 0.969, 0.972)
//                glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
                
//                glColor3f(1,1,1);
//                GLfloat ambient[4] = {1,1,1,1};
                Eigen::Vector4f ambient(0.9, 0.9, 0.9, 1.0 );
                Eigen::Vector4f diffuse(0.83, 0.85, 0.87, 1.0);
                
                GLfloat specular[4] = {0,0,0,1};
                glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, ambient.data() );
                glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse.data() );
                glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specular );
                
                for(int ax=0; ax<3; ++ax) {
                    Eigen::Vector3f n(0,0,0); n(ax) = 1;
                    Eigen::Vector3f nc = T_co.topLeftCorner<3,3>() * n;
                    DrawFace(bmin,bmax,ax,nc(2)>0 ? 1 : -1);
                }
                
                Eigen::Array4f  papermod(0.686, 0.874, 0.929, 1.0);
                Eigen::Vector4f lambient = ambient.array() * papermod;
                Eigen::Vector4f ldiffuse = diffuse.array() * papermod;
//                GLfloat lambient[4] = {0.2, 0.2, 0.2, 1.0 };
//                GLfloat ldiffuse[4] = {0.686, 0.874, 0.929, 1.0 };
                GLfloat lspecular[4] = {0,0,0,1};
                glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, lambient.data() );
                glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, ldiffuse.data() );
                glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, lspecular );                                

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
            bbox.ScaleFromCenter(m_box_grow_factor);
            GLObject::m_aabb = bbox;
            DrawOpenBox(bbox, m_colorPlane, m_colorLines);
        }

        inline void SetPlaneColor(const GLColor& color) {
            m_colorPlane = color;
        }

        inline void SetLineColor(const GLColor& color) {
            m_colorLines = color;
        }

protected:
        Eigen::Vector3d m_box_grow_factor;
        double m_interval;
        int m_ticks_per_interval;
        GLColor m_colorPlane;
        GLColor m_colorLines;
};

} // SceneGraph

#endif

