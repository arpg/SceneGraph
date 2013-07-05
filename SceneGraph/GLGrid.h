#ifndef _GL_GRID_H_
#define _GL_GRID_H_

#include <SceneGraph/GLObject.h>
#include <SceneGraph/GLColor.h>
#include <pangolin/gldraw.h>

namespace SceneGraph
{

/////////////////////////////////////////////////////////////////////////////
class GLGrid : public GLObject
{
    public:
        GLGrid(int numLines = 50, float lineSpacing = 2.0, bool perceptable = false)
            :   GLObject("Grid"),
                m_nNumLines( numLines ),
                m_fLineSpacing( lineSpacing ),
                m_colorPlane( 0.4f, 0.4f, 0.4f, 1.0f ),
                m_colorLines( 0.5f, 0.5f, 0.5f, 1.0 )
        {
            m_bPerceptable = perceptable;
            mT_op = Eigen::Matrix4d::Identity();
            ComputeBounds();
        }

        void SetNumLines( int nNumLines )
        {
            m_nNumLines = nNumLines;
            ComputeBounds();
        }

        void SetLineSpacing( float fLineSpacing )
        {
            m_fLineSpacing = fLineSpacing;
            ComputeBounds();
        }
        
        void SetColors(const GLColor& planeColor, const GLColor& lineColor)
        {
            m_colorPlane = planeColor;
            m_colorLines = lineColor;
        }

        // from mvl dispview
        static inline void DrawGridZ0( bool filled, int numLines, float lineSpacing, GLColor colorPlane, GLColor colorLines)
        {
            pangolin::GlState gl;
            
            // Prevent Z-Fighting between plane and lines
            glPolygonOffset( 1.0, 1.0 );
            gl.glEnable(GL_POLYGON_OFFSET_FILL);
            gl.glDisable(GL_CULL_FACE);
           
//            glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
            GLfloat ambient[4] = {1,1,1,1};
            GLfloat diffuse[4] = {0,0,0,1};
            GLfloat specular[4] = {0,0,0,1};
            glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, ambient );
            glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse );
            glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specular );

            glNormal3f( 0,0,-1 );

            const float halfsize = lineSpacing*numLines;

            if( filled ) {
                colorPlane.Apply();
                glRectf(-halfsize, -halfsize, halfsize, halfsize);
                
                // Don't overwrite this depth when drawing lines:
                gl.glDepthMask(GL_FALSE);
            }

            {
                colorLines.Apply();
                for(int i = -numLines; i <= numLines; i++){
                    if(i) {
                        pangolin::glDrawLine(
                                    halfsize, i*lineSpacing, 0.0,
                                    -halfsize, i*lineSpacing, 0.0
                                    );
                        pangolin::glDrawLine(
                                    i*lineSpacing,  halfsize, 0.0,
                                    i*lineSpacing, -halfsize, 0.0
                                    );
                    }
                }

                glColor4ub(255, 0, 0, 128);
                pangolin::glDrawLine( halfsize , 0.0, 0.0, -halfsize , 0.0, 0.0);

                glColor4ub(0, 255, 0, 128);
                pangolin::glDrawLine( 0.0,  halfsize, 0.0,  0.0, -halfsize, 0.0);
            }
        }

        void DrawCanonicalObject(void)
        {
            glPushMatrix();
            glMultMatrixd( mT_op.data() );
            DrawGridZ0( m_bPerceptable, m_nNumLines, m_fLineSpacing, m_colorPlane, m_colorLines);
            glPopMatrix();
        }

        inline void SetPlaneColor(const GLColor& color) {
            m_colorPlane = color;
        }

        inline void SetLineColor(const GLColor& color) {
            m_colorLines = color;
        }

        // nd_o = (n_o / d_o)
        // n_o : Normal of plane with respect to object frame of ref
        // d_o : Distance of closest approach with origin of object frame
        inline void SetPlane(const Eigen::Vector3d& nd_o)
        {
            const double d = 1.0 / nd_o.norm();
            const Eigen::Vector3d n = d * nd_o;
            mT_op.block<3,3>(0,0) = Rotation_a2b(Eigen::Vector3d(0,0,-1),n);
            mT_op.block<3,1>(0,3) = -d*n;
            ComputeBounds();
        }

protected:
        void ComputeBounds()
        {
            const float halfsize = m_fLineSpacing*m_nNumLines;
            m_aabb.Clear();
            m_aabb.Insert(mT_op,Eigen::Vector3d(-halfsize,-halfsize,0));
            m_aabb.Insert(mT_op,Eigen::Vector3d(-halfsize,halfsize,0));
            m_aabb.Insert(mT_op,Eigen::Vector3d(halfsize,-halfsize,0));
            m_aabb.Insert(mT_op,Eigen::Vector3d(halfsize,halfsize,0));
        }

        int m_nNumLines;
        float m_fLineSpacing;

        GLColor m_colorPlane;
        GLColor m_colorLines;

        // Plane to object transform (represents canonical basis for plane)
        Eigen::Matrix4d mT_op;
};

} // SceneGraph

#endif

