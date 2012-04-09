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

    void m_gldrawCircle(float r)
    {
        const float d2r = 3.14159/180; //must be in radians
        glBegin(GL_LINE_LOOP);

//      glColor3f(0.0, 1.0, 0.0); //green
        for(int i = 0; i < 360; i++)
        {
            float theta = i * d2r;
            glVertex3f(0.0, cos(theta)*r, sin(theta)*r);
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

    //x-axis
        glBegin(GL_LINES);
        glColor3f(0.0, 1.0, 0.0); //green
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(-1.3, 0.0, 0.0);
        glEnd();
        glPushMatrix();
        glPushName(13);
        m_gldrawCircle(1.0);
        glPopMatrix();
    //y-axis
        glBegin(GL_LINES);
        glColor3f(1.0, 0.0, 0.0); //red
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, -1.3, 0.0);
        glEnd();

        glPushMatrix();
        glPushName(23);
        glRotatef(90.0, 0.0, 0.0, 1.0);
        m_gldrawCircle(1.0);
        glPopMatrix();
    //z-axis
        glBegin(GL_LINES);
        glColor3f(0.0, 0.0, 1.0);//blue
        glVertex3f(0.0, 0.0, 0.0);//origin
        glVertex3f(0.0, 0.0, -1.3);
        glEnd();

        glPushMatrix();
        glPushName(33);
        glRotatef(90.0, 0.0, 1.0, 0.0);
        m_gldrawCircle(1.0);
        glPopMatrix();

        glLineWidth(1.0);

        glPopMatrix(); //pop back to origin

        glDisable( GL_BLEND );

    }

private:
    unsigned int m_nWayPointId;
    unsigned int m_nFrontId;
    unsigned int m_nBaseId;
    unsigned int m_nSelectedId;

    std::string m_sLabel;
    Eigen::Vector2i m_nLabelPos;
    Eigen::Vector6d m_dPose;
};

#endif // GLAXIS_H
