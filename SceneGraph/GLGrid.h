#ifndef _GL_GRID_H_
#define _GL_GRID_H_

#include <SceneGraph/GLObject.h>
#include <SceneGraph/GLColor.h>

namespace SceneGraph
{

/////////////////////////////////////////////////////////////////////////////
class GLGrid : public GLObject
{
    public:
        GLGrid(int numLines = 50, float lineSpacing = 2.0, bool perceptable = false)
            : GLObject("Grid"), m_iNumLines(numLines), m_fLineSpacing(lineSpacing),
              m_colorPlane(90,90,90,255), m_colorLines(132,132,132,128)
        {
            m_bPerceptable = perceptable;
            mT_op = Eigen::Matrix4d::Identity();
        }

        // from mvl dispview
        static inline void DrawGridZ0(bool filled, int numLines, float lineSpacing, GLColor colorPlane, GLColor colorLines)
        {
            glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
            {
//                glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);

                GLfloat ambient[4] = {1,1,1,1};
//                GLfloat diffuse[4] = {0.1,0.1,0.1,1};
                GLfloat diffuse[4] = {0,0,0,1};
                GLfloat specular[4] = {0,0,0,1};
                glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, ambient );
                glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse );
                glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specular );

//                glDisable(GL_LIGHTING);

                glNormal3f(0,0,-1);

                const float halfsize = lineSpacing*numLines;

                if(filled) {
                    colorPlane.Apply();
                    glBegin(GL_TRIANGLE_STRIP);
                    glVertex3f( -halfsize , -halfsize, 0.0);
                    glVertex3f( +halfsize , -halfsize, 0.0);
                    glVertex3f( -halfsize , +halfsize, 0.0);
                    glVertex3f( +halfsize , +halfsize, 0.0);
                    glEnd();
                }

                glBegin(GL_LINES);
                {
                    for(int i = -numLines; i < numLines; i++){
                        colorLines.Apply();
                        glVertex3f( halfsize, i*lineSpacing, 0.0);
                        glVertex3f(-halfsize, i*lineSpacing, 0.0);
                        glVertex3f(i*lineSpacing,  halfsize, 0.0);
                        glVertex3f(i*lineSpacing, -halfsize, 0.0);
                    }

                    glColor4ub(255, 0, 0, 128);
                    glVertex3f( halfsize , 0.0, 0.0);
                    glVertex3f(-halfsize , 0.0, 0.0);

                    glColor4ub(0, 255, 0, 128);
                    glVertex3f( 0.0,  halfsize, 0.0);
                    glVertex3f( 0.0, -halfsize, 0.0);
                }
                glEnd();
            }
            glPopAttrib();
        }

        void DrawCanonicalObject(void)
        {
            glPushMatrix();
            glMultMatrixd(mT_op.data());
            DrawGridZ0(m_bPerceptable, m_iNumLines, m_fLineSpacing, m_colorPlane, m_colorLines);
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
        }

protected:
        int m_iNumLines;
        float m_fLineSpacing;

        GLColor m_colorPlane;
        GLColor m_colorLines;

        // Plane to object transform (represents canonical basis for plane)
        Eigen::Matrix4d mT_op;
};

} // SceneGraph

#endif

