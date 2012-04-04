#ifndef GLAXIS_H
#define GLAXIS_H

#include <SimpleGui/GLObject.h>

///////////////////////////////////////////////////////////////////////////////
class GLAxis : public GLObject
{
public:

    GLAxis()
    {
        m_sObjectName = "Axis";
    }

    //dispview
    void draw(void)
    {
        glPushMatrix();

        //translate position
        glTranslatef(0.0, 0.0, -1.0); //this will be centered at object's centroid

        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        glEnable( GL_BLEND );
        glDisable( GL_LIGHTING );

        //draw lines
        glBegin(GL_LINES);
        glColor3f(0.0, 0.0, 1.0);//set color to blue
 //       glLineWidth(2.0);
        glVertex3f(0.0, 0.0, 0.0);//origin
        glVertex3f(0.0, 0.0, -1.0);//z-axis

        glColor3f(0.0, 1.0, 0.0); //green
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(-1.0, 0.0, 0.0); //x-axis

        glColor3f(1.0, 0.0, 0.0); //red
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, -1.0, 0.0); //y-axis

        glEnd(); //end draw lines

        glPopMatrix(); //pop back to origin

        //draw rings
        //glPushMatrix();

        glDisable( GL_BLEND );

    }
};

#endif // GLAXIS_H
