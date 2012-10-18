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
          m_resizable(false),
          x_min(-1), y_min(-1), z_min(-1),
          x_max(1), y_max(1), z_max(1)
    {
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

    void DrawAxisAlignedBox(
            float x_min, float x_max,
            float y_min, float y_max,
            float z_min, float z_max
    ) {
        glColor4f(1, 0, 0, 1);
        glPushName(x_label);
        glBegin(GL_LINES);
        glVertex3d(x_min, y_min, z_min); glVertex3d(x_max, y_min, z_min);
        glVertex3d(x_min, y_max, z_min); glVertex3d(x_max, y_max, z_min);
        glVertex3d(x_min, y_max, z_max); glVertex3d(x_max, y_max, z_max);
        glVertex3d(x_min, y_min, z_max); glVertex3d(x_max, y_min, z_max);
        glEnd();
        glPopName();

        glColor4f(0, 1, 0, 1);
        glPushName(y_label);
        glBegin(GL_LINES);
        glVertex3d(x_min, y_min, z_min); glVertex3d(x_min, y_max, z_min);
        glVertex3d(x_max, y_min, z_min); glVertex3d(x_max, y_max, z_min);
        glVertex3d(x_max, y_min, z_max); glVertex3d(x_max, y_max, z_max);
        glVertex3d(x_min, y_min, z_max); glVertex3d(x_min, y_max, z_max);
        glEnd();
        glPopName();

        glColor4f(0, 0, 1, 1);
        glPushName(z_label);
        glBegin(GL_LINES);
        glVertex3d(x_min, y_min, z_min); glVertex3d(x_min, y_min, z_max);
        glVertex3d(x_max, y_min, z_min); glVertex3d(x_max, y_min, z_max);
        glVertex3d(x_max, y_max, z_min); glVertex3d(x_max, y_max, z_max);
        glVertex3d(x_min, y_max, z_min); glVertex3d(x_min, y_max, z_max);
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
                    x_min *= mul;
                    x_max *= mul;
                    return true;
                }else if(pickId == y_label) {
                    y_min *= mul;
                    y_max *= mul;
                    return true;
                }else if(pickId == z_label) {
                    z_min *= mul;
                    z_max *= mul;
                    return true;
                }
            }
//        }

        return false;
    }

    void DrawCanonicalObject()
    {
        glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_LIGHTING);
        DrawAxisAlignedBox(x_min, x_max, y_min, y_max, z_min, z_max);
        glPopAttrib();
    }

    void SetBounds(float minx, float miny, float minz, float maxx, float maxy, float maxz)
    {
        x_min = minx; y_min = miny; z_min = minz;
        x_max = maxx; y_max = maxy; z_max = maxz;
    }

protected:
    bool m_resizable;
    float x_min,y_min,z_min;
    float x_max,y_max,z_max;
    int   x_label, y_label, z_label;
};

} // SceneGraph

#endif // GLAXISALIGNEDBOX_H
