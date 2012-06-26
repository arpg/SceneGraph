#ifndef _GL_GRID_H_
#define _GL_GRID_H_

#include <SceneGraph/GLObject.h>

namespace SceneGraph
{

/////////////////////////////////////////////////////////////////////////////
class GLGrid : public GLObject
{
    public:

        GLGrid()
        {
            m_sObjectName = "Grid";
            m_bPerceptable = false;
        }

        // from mvl dispview
        void draw(void)
        {
            #define NLINES 50
            #define LDELTA 2

            glPushAttrib(GL_ENABLE_BIT);
            {
                glDisable(GL_LIGHTING);

                glBegin(GL_LINES);
                {
                    for(int i = -NLINES; i < NLINES; i++){
                        glColor4ub(132, 132, 132, 50);
                        glVertex3f( LDELTA*NLINES, i*LDELTA, 0.0);
                        glVertex3f(-LDELTA*NLINES, i*LDELTA, 0.0);

                        glColor4ub(132, 132, 132, 50);
                        glVertex3f(i*LDELTA,  LDELTA*NLINES, 0.0);
                        glVertex3f(i*LDELTA, -LDELTA*NLINES, 0.0);
                    }

                    glColor4ub(255, 0, 0, 128);
                    glVertex3f( LDELTA*NLINES , 0.0, 0.0);
                    glVertex3f(-LDELTA*NLINES , 0.0, 0.0);

                    glColor4ub(0, 255, 0, 128);
                    glVertex3f( 0.0,  LDELTA*NLINES, 0.0);
                    glVertex3f( 0.0, -LDELTA*NLINES, 0.0);
                }
                glEnd();
            }
            glPopAttrib();

        }

};

} // SceneGraph

#endif

