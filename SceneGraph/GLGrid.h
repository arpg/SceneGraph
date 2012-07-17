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
        }

        // from mvl dispview
        void DrawCanonicalObject(void)
        {
            glPushAttrib(GL_ENABLE_BIT);
            {
                glDisable(GL_LIGHTING);

                if(m_bPerceptable) {
                    glColor4ub(90, 90, 90, 128);
                    glBegin(GL_TRIANGLE_STRIP);
                    glVertex3f( -m_fLineSpacing*m_iNumLines , -m_fLineSpacing*m_iNumLines, 0.0);
                    glVertex3f( +m_fLineSpacing*m_iNumLines , -m_fLineSpacing*m_iNumLines, 0.0);
                    glVertex3f( -m_fLineSpacing*m_iNumLines , +m_fLineSpacing*m_iNumLines, 0.0);
                    glVertex3f( +m_fLineSpacing*m_iNumLines , +m_fLineSpacing*m_iNumLines, 0.0);
                    glEnd();
                }

                glBegin(GL_LINES);
                {
                    for(int i = -m_iNumLines; i < m_iNumLines; i++){
                        glColor4ub(132, 132, 132, 50);
                        glVertex3f( m_fLineSpacing*m_iNumLines, i*m_fLineSpacing, 0.0);
                        glVertex3f(-m_fLineSpacing*m_iNumLines, i*m_fLineSpacing, 0.0);

                        glColor4ub(132, 132, 132, 50);
                        glVertex3f(i*m_fLineSpacing,  m_fLineSpacing*m_iNumLines, 0.0);
                        glVertex3f(i*m_fLineSpacing, -m_fLineSpacing*m_iNumLines, 0.0);
                    }

                    glColor4ub(255, 0, 0, 128);
                    glVertex3f( m_fLineSpacing*m_iNumLines , 0.0, 0.0);
                    glVertex3f(-m_fLineSpacing*m_iNumLines , 0.0, 0.0);

                    glColor4ub(0, 255, 0, 128);
                    glVertex3f( 0.0,  m_fLineSpacing*m_iNumLines, 0.0);
                    glVertex3f( 0.0, -m_fLineSpacing*m_iNumLines, 0.0);
                }
                glEnd();
            }
            glPopAttrib();

        }

protected:
        int m_iNumLines;
        float m_fLineSpacing;

};

} // SceneGraph

#endif

