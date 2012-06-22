#ifndef _GL_GRID_H_
#define _GL_GRID_H_

#include <SimpleGui/GLObject.h>


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

            int i;

            #define NLINES 50
            #define LDELTA 2

            glPushAttrib(GL_ENABLE_BIT);
            glPushMatrix();

            glEnable( GL_BLEND );
            glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
            //    glBlendFunc( GL_SRC_ALPHA_SATURATE,  GL_ONE );

//            glColor4f( 1,1,1,0.1 );

            glDisable( GL_LIGHTING );
//            glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
//            glEnable( GL_BLEND );
//            glDepthMask (GL_TRUE);

            glBegin(GL_LINES);

            for(i = -NLINES; i < NLINES; i++){
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

            glEnd();


            glPopMatrix();
            glPopAttrib();

        }

};

#endif

