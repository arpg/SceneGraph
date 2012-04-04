#ifndef GLAXIS_H
#define GLAXIS_H

#include <SimpleGui/GLObject.h>
#include <math.h>

///////////////////////////////////////////////////////////////////////////////
class GLAxis : public GLObject
{
public:

    GLAxis()
    {
        m_sObjectName = "Axis";
    }

    void drawCircle(float r, char axis)
    {
        const float d2r = 3.14159/180; //must be in radians
        glBegin(GL_LINE_LOOP);

        if (axis == 'x')
        {
            glColor3f(0.0, 1.0, 0.0); //green
            for(int i = 0; i < 360; i++)
            {
                float theta = i * d2r;
                glVertex3f(0.0, cos(theta)*r, sin(theta)*r);
            }
        }
        else if (axis == 'y')
        {
            glColor3f(1.0, 0.0, 0.0); //red
            for(int i = 0; i < 360; i++)
            {
                float theta = i * d2r;
                glVertex3f(sin(theta)*r, 0.0, cos(theta)*r);
            }
        }

        else //axis = z
        {

            glColor3f(0.0, 0.0, 1.0); //blue
            for(int i = 0; i < 360; i++)
            {
                float theta = i * d2r;
                glVertex3f(cos(theta)*r, sin(theta)*r, 0.0);
            }
        }

        glEnd();

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
        glLineWidth(5.0); //for some reason this freaks things out
        glBegin(GL_LINES);

    //x-axis
        glColor3f(0.0, 1.0, 0.0); //green
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(-1.2, 0.0, 0.0);

    //y-axis
        glColor3f(1.0, 0.0, 0.0); //red
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, -1.2, 0.0);

    //z-axis
        glColor3f(0.0, 0.0, 1.0);//blue
        glVertex3f(0.0, 0.0, 0.0);//origin
        glVertex3f(0.0, 0.0, -1.2);

        glEnd(); //end draw lines

        //draw rings
        drawCircle(1.0, 'x');
        drawCircle(1.0, 'y');
        drawCircle(1.0, 'z');

        glLineWidth(1.0);

        glPopMatrix(); //pop back to origin

        glDisable( GL_BLEND );

    }
};

#endif // GLAXIS_H
