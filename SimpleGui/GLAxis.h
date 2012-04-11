#ifndef GLAXIS_H
#define GLAXIS_H

#include <SimpleGui/GLObject.h>
#include <math.h>

namespace Eigen{
    typedef Matrix<double,6,1> Vector6d;
    typedef Matrix<double,5,1> Vector5d;
}
#define X_AXIS  1
#define Y_AXIS  2
#define Z_AXIS  3
#define RING  10
#define LINE  20

///////////////////////////////////////////////////////////////////////////////
class GLAxis : public GLObject
{
public:

    GLAxis()
    {
        m_sObjectName = "gizmo";
        m_nBaseId = -1, m_nXLineId = -1, m_nXRingId = -1, m_nYLineId = -1, m_nYRingId = -1, m_nZLineId = -1, m_nZRingId = -1;
        m_bInitDone = false;
        m_dPose = Eigen::Vector6d::Zero();
    }


    void Init()
    {
        SetName("Gizmo");
        m_nBaseId = AllocSelectionId();
        m_nXLineId = AllocSelectionId();
        m_nXRingId = AllocSelectionId();
        m_nYLineId = AllocSelectionId();
        m_nYRingId = AllocSelectionId();
        m_nZLineId = AllocSelectionId();
        m_nZRingId = AllocSelectionId();
        m_bInitDone = true;
    }

    void select( unsigned int nId )
    {
        Eigen::Vector3d v = Window()->GetPosUnderCursor();
        m_nSelectedId = nId;

        if(nId == m_nBaseId)
        {
            printf("gizmo selected\n");
            m_dPose[0] = v[0];
            m_dPose[1] = v[1];
            m_dPose[2] = v[2];
            UnSelect(m_nBaseId);
        }
        if (nId == m_nXLineId)
        {
            printf("XLine Selected\n");
        }
        else if (nId == m_nXRingId)
        {
            printf("XRing Selected\n");
        }
        else if (nId == m_nYLineId)
        {
            printf("YLine Selected\n");
        }
        else if (nId == m_nYRingId)
        {
            printf("YRing Selected\n");
        }
        else if (nId == m_nZLineId)
        {
            printf("ZLine Selected\n");
        }
        else if (nId == m_nZRingId)
        {
            printf("ZRing Selected\n");
        }

    }

    void drag ()
    {

    }

    void release()
    {

    }

    void MouseOver(unsigned int nId )
    {
        printf("mouseover!\n");
    }



    //dispview
    void draw(void)
    {
        if( !m_bInitDone)
        {
            Init();
        }
        glPushMatrix();

        //translate position
        glTranslatef(0.0, 0.0, -1.0); //this will be centered at object's centroid

        //general rendering settings
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        glEnable( GL_BLEND );
        glDisable( GL_LIGHTING );

        //thick lines
        glLineWidth(5.0);
//        glPushName(m_nBaseId);

    //x-axis
        glColor4f(0.0, 1.0, 0.0, 0.3); //green
        if (IsSelected(m_nXLineId))
        {
            UnSelect(m_nXLineId);
            glColor4f(0.0, 1.0, 0.0, 1.0); //green
        }
        glPushName(m_nXLineId);
        glBegin(GL_LINES);

        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(-1.5, 0.0, 0.0);
        glEnd();
        glPopName();
        glColor4f(0.0, 1.0, 0.0, 0.3); //green
        if (IsSelected(m_nXRingId))
        {
            UnSelect(m_nXRingId);
            glColor4f(0.0, 1.0, 0.0, 1.0); //green
        }
        glPushName(m_nXRingId);
        m_gldrawCircle(1.0);
        glPopName();

    //y-axis
        glColor4f(1.0, 0.0, 0.0, 0.3); //red
        if (IsSelected(m_nYLineId))
        {
            UnSelect(m_nYLineId);
            glColor4f(1.0, 0.0, 0.0, 1.0); //red
        }
        glPushName(m_nYLineId);
        glBegin(GL_LINES);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, -1.5, 0.0);
        glEnd();
        glPopName();
        glPushMatrix();
        glRotatef(90.0, 0.0, 0.0, 1.0);
        glColor4f(1.0, 0.0, 0.0, 0.3); //red
        if (IsSelected(m_nYRingId))
        {
            UnSelect(m_nYRingId);
            glColor4f(1.0, 0.0, 0.0, 1.0); //red
        }
        glPushName(m_nYRingId);
        m_gldrawCircle(1.0);
        glPopName();
        glPopMatrix();

    //z-axis
        glColor4f(0.0, 0.0, 1.0, 0.3);//blue
        if (IsSelected(m_nZLineId))
        {
            UnSelect(m_nZLineId);
            glColor4f(0.0, 0.0, 1.0, 1.0);//blue
        }
        glPushName(m_nZLineId);
        glBegin(GL_LINES);

        glVertex3f(0.0, 0.0, 0.0);//origin
        glVertex3f(0.0, 0.0, -1.5);
        glEnd();
        glPopName();
        glPushMatrix();
        glRotatef(90.0, 0.0, 1.0, 0.0);
        glColor4f(0.0, 0.0, 1.0, 0.3);//blue
        if (IsSelected(m_nZRingId))
        {
            UnSelect(m_nZRingId);
            glColor4f(0.0, 0.0, 1.0, 1.0);//blue
        }
        glPushName(m_nZRingId);
        m_gldrawCircle(1.0);
        glPopName();
        glPopMatrix();
//        glPopName(); //end of axis naming

        glLineWidth(1.0);

       glPopMatrix(); //pop back to origin

        glDisable( GL_BLEND );

    }

    void m_gldrawCircle(float r)
    {
        const float d2r = 3.14159/180; //must be in radians
        glBegin(GL_LINE_LOOP);

        for(int i = 0; i < 360; i++)
        {
            float theta = i * d2r;
            glVertex3f(0.0, cos(theta)*r, sin(theta)*r);
        }
        glEnd();

    }


private:
    unsigned int m_nBaseId;
    unsigned int m_nSelectedId;
    unsigned int m_nXLineId;
    unsigned int m_nXRingId;
    unsigned int m_nYLineId;
    unsigned int m_nYRingId;
    unsigned int m_nZLineId;
    unsigned int m_nZRingId;

    bool m_bInitDone;

    Eigen::Vector6d m_dPose;
};

#endif // GLAXIS_H
