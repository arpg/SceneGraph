#ifndef _GL_GRID_H_
#define _GL_GRID_H_

#include <SceneGraph/GLObject.h>

namespace SceneGraph
{

/////////////////////////////////////////////////////////////////////////////
class GLGrid : public GLObject
{
    public:
        GLGrid(int numLines = 50, float lineSpacing = 2.0, bool perceptable = false)
            : GLObject("Grid"), m_iNumLines(numLines), m_fLineSpacing(lineSpacing)
        {
            m_bPerceptable = perceptable;
            mT_op = Eigen::Matrix4d::Identity();
        }

        // from mvl dispview
        static inline void DrawGridZ0(bool filled, int numLines, float lineSpacing)
        {
            glPushAttrib(GL_ENABLE_BIT);
            {
                glDisable(GL_LIGHTING);

                const float halfsize = lineSpacing*numLines;

                if(filled) {
                    glColor4ub(90, 90, 90, 128);
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
                        glColor4ub(132, 132, 132, 255);
                        glVertex3f( halfsize, i*lineSpacing, 0.0);
                        glVertex3f(-halfsize, i*lineSpacing, 0.0);

                        glColor4ub(132, 132, 132, 255);
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
            DrawGridZ0(m_bPerceptable, m_iNumLines, m_fLineSpacing);
            glPopMatrix();
        }

        // nd_o = (n_o / d_o)
        // n_o : Normal of plane with respect to object frame of ref
        // d_o : Distance of closest approach with origin of object frame
        void SetPlane(Eigen::Vector3d nd_o)
        {
            const double d = 1.0 / nd_o.norm();
            const Eigen::Vector3d n = d * nd_o;
            mT_op.block<3,3>(0,0) = Rotation_a2b(Eigen::Vector3d(0,0,-1),n);
            mT_op.block<3,1>(0,3) = -d*n;
        }

protected:

        Eigen::Matrix3d Rotation_a2b(const Eigen::Vector3d& a, const Eigen::Vector3d& b)
        {
            Eigen::Vector3d n = a.cross(b);

            if(n.squaredNorm() == 0) {
                // TODO: Should this be identity?
                return Eigen::Matrix3d::Identity();
            }

            n.normalize();
            Eigen::Matrix3d R1;
            R1.col(0) = a.normalized();
            R1.col(1) = n;
            R1.col(2) = n.cross(R1.col(0));

            Eigen::Matrix3d M;
            M.col(0) = b.normalized();
            M.col(1) = n;
            M.col(2) = n.cross(M.col(0));
            M = M * R1.transpose();
            return M;
        }

        int m_iNumLines;
        float m_fLineSpacing;

        // Plane to object transform (represents canonical basis for plane)
        Eigen::Matrix4d mT_op;
};

} // SceneGraph

#endif

