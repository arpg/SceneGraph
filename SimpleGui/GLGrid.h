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
#define LDELTA 5

            glPushMatrix();
            glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
            //    glBlendFunc( GL_SRC_ALPHA_SATURATE,  GL_ONE );
            glEnable( GL_BLEND );
//            glColor4f( 1,1,1,0.1 );

            glDisable( GL_LIGHTING );
//            glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
//            glEnable( GL_BLEND );
//            glDepthMask (GL_TRUE);

            glBegin(GL_LINES);

            for(i = -NLINES; i < NLINES; i++){
                glColor4ub(132, 132, 132, 195);
                glVertex3f( LDELTA*NLINES, i*LDELTA, 0.0);
                glVertex3f(-LDELTA*NLINES, i*LDELTA, 0.0);

                glColor4ub(132, 132, 132, 195);
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

            /*
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );

            // so we generate depth on the ground plane
            glEnable( GL_DEPTH_TEST );
            glBegin( GL_QUADS );
            glColor4ub( 0, 0, 0, 0);
            glVertex3f(  LDELTA*NLINES ,  LDELTA*NLINES, 0.01);
            glVertex3f( -LDELTA*NLINES ,  LDELTA*NLINES, 0.01);
            glVertex3f( -LDELTA*NLINES , -LDELTA*NLINES, 0.01);
            glVertex3f(  LDELTA*NLINES , -LDELTA*NLINES, 0.01);
            glEnd();
            */
            glPopMatrix();
            /*
            Eigen::Vector3d v = GetPosUnderCursor();
            glPointSize( 5 );
            glBegin( GL_POINTS );
            glColor4ub( 255, 255, 255, 255 );
            glVertex3f( v[0],  v[1], v[2] );
            glEnd();
            */

//            glDisable( GL_BLEND );
//            glEnable( GL_LIGHTING );
        }

};

#endif

