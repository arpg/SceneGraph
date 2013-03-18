#ifndef GLAXISALIGNEDBOX_H
#define GLAXISALIGNEDBOX_H

#include <SceneGraph/GLObject.h>

namespace SceneGraph
{

///////////////////////////////////////////////////////////////////////////////
class GLAxisAlignedBox : public GLObject
{
public:

    GLAxisAlignedBox()
        : GLObject("AxisAlignedBox"),
          m_resizable(false)
    {
        m_aabb.Min() = Eigen::Vector3d(-1,-1,-1);
        m_aabb.Max() = Eigen::Vector3d(1,1,1);
        m_bPerceptable = false;
        m_bIsSelectable = true;
        x_label = AllocSelectionId();
        y_label = AllocSelectionId();
        z_label = AllocSelectionId();

    }

    void SetResizable(bool resizable = true)
    {
        m_resizable = resizable;
    }

    void DrawAxisAlignedBox( const AxisAlignedBoundingBox& bbox ) {
        const Eigen::Vector3d mn = bbox.Min();
        const Eigen::Vector3d mx = bbox.Max();
        
        glColor4f(1, 0, 0, 1);
        glPushName(x_label);
        glBegin(GL_LINES);
        glVertex3d(mn(0), mn(1), mn(2)); glVertex3d(mx(0), mn(1), mn(2));
        glVertex3d(mn(0), mx(1), mn(2)); glVertex3d(mx(0), mx(1), mn(2));
        glVertex3d(mn(0), mx(1), mx(2)); glVertex3d(mx(0), mx(1), mx(2));
        glVertex3d(mn(0), mn(1), mx(2)); glVertex3d(mx(0), mn(1), mx(2));
        glEnd();
        glPopName();

        glColor4f(0, 1, 0, 1);
        glPushName(y_label);
        glBegin(GL_LINES);
        glVertex3d(mn(0), mn(1), mn(2)); glVertex3d(mn(0), mx(1), mn(2));
        glVertex3d(mx(0), mn(1), mn(2)); glVertex3d(mx(0), mx(1), mn(2));
        glVertex3d(mx(0), mn(1), mx(2)); glVertex3d(mx(0), mx(1), mx(2));
        glVertex3d(mn(0), mn(1), mx(2)); glVertex3d(mn(0), mx(1), mx(2));
        glEnd();
        glPopName();

        glColor4f(0, 0, 1, 1);
        glPushName(z_label);
        glBegin(GL_LINES);
        glVertex3d(mn(0), mn(1), mn(2)); glVertex3d(mn(0), mn(1), mx(2));
        glVertex3d(mx(0), mn(1), mn(2)); glVertex3d(mx(0), mn(1), mx(2));
        glVertex3d(mx(0), mx(1), mn(2)); glVertex3d(mx(0), mx(1), mx(2));
        glVertex3d(mn(0), mx(1), mn(2)); glVertex3d(mn(0), mx(1), mx(2));
        glEnd();
        glPopName();
    }

    bool Mouse(int button, const Eigen::Vector3d& /*win*/, const Eigen::Vector3d& /*obj*/, const Eigen::Vector3d& /*normal*/, bool /*pressed*/, int /*button_state*/, int pickId)
    {
        // TODO: Make this much better / more flexible

//        if(glutGetModifiers() & GLUT_ACTIVE_SHIFT){
            if(m_resizable && (button == MouseWheelUp || button == MouseWheelDown) ) {
                const float mul = (button == MouseWheelUp) ? 1.01 : 0.99;
                if(pickId == x_label) {
                    m_aabb.Min()(0) *= mul;
                    m_aabb.Max()(0) *= mul;
                    return true;
                }else if(pickId == y_label) {
                    m_aabb.Min()(1) *= mul;
                    m_aabb.Max()(1) *= mul;
                    return true;
                }else if(pickId == z_label) {
                    m_aabb.Min()(2) *= mul;
                    m_aabb.Max()(2) *= mul;
                    return true;
                }
            }
//        }

        return false;
    }

    void DrawCanonicalObject()
    {
        DrawAxisAlignedBox(m_aabb);
    }

    void SetBounds(Eigen::Vector3d boxmin, Eigen::Vector3d boxmax)
    {
        m_aabb.Min() = boxmin;
        m_aabb.Max() = boxmax;
    }

    void SetBounds(float minx, float miny, float minz, float maxx, float maxy, float maxz)
    {
        SetBounds(Eigen::Vector3d(minx,miny,minz), Eigen::Vector3d(maxx,maxy,maxz) );
    }

protected:
    bool m_resizable;
    int   x_label, y_label, z_label;
};

} // SceneGraph

#endif // GLAXISALIGNEDBOX_H
