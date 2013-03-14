#ifndef GLMOVABLEAXIS_H
#define GLMOVABLEAXIS_H

#include <SceneGraph/GLObject.h>

namespace SceneGraph
{

///////////////////////////////////////////////////////////////////////////////
class GLMovableAxis : public GLObject
{
public:

    GLMovableAxis(float axisSize = 1.0f, bool rotatable = true, bool translatable = true)
        : GLObject("Axis"), m_axisSize(axisSize), m_rotatable(rotatable), m_translatable(translatable)
    {
        m_bIsSelectable = translatable | rotatable;
        m_bPerceptable = false;
        x_label = AllocSelectionId();
        y_label = AllocSelectionId();
        z_label = AllocSelectionId();
    }

    bool Mouse(int button, const Eigen::Vector3d& /*win*/, const Eigen::Vector3d& /*obj*/, const Eigen::Vector3d& /*normal*/, bool /*pressed*/, int button_state, int pickId)
    {
        if((button == MouseWheelUp || button == MouseWheelDown) ) {
            float scale = (button == MouseWheelUp) ? 0.01 : -0.01;
            if(button_state & KeyModifierShift) scale /= 10;

            Eigen::Matrix<double,6,1> T_po = GetPose();
            Eigen::Matrix<double,6,1> T_on = Eigen::Matrix<double,6,1>::Zero();

            if(button_state & KeyModifierCtrl) {
                if(m_rotatable) {
                    // rotate
                    if(pickId == x_label) {
                        T_on(3) += scale;
                    }else if(pickId == y_label) {
                        T_on(4) += scale;
                    }else if(pickId == z_label) {
                        T_on(5) += scale;
                    }else{
                        return false;
                    }
                }
            }else{
                if(m_translatable) {
                    // translate
                    if(pickId == x_label) {
                        T_on(0) += m_axisSize*scale;
                    }else if(pickId == y_label) {
                        T_on(1) += m_axisSize*scale;
                    }else if(pickId == z_label) {
                        T_on(2) += m_axisSize*scale;
                    }else{
                        return false;
                    }
                }
            }
            Eigen::Matrix4d T_pn = (GLCart2T(T_po) * GLCart2T(T_on));
            SetPose(T_pn);
            return true;
        }

        return false;
    }

    void DrawCanonicalObject()
    {
        glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT);
        glDisable( GL_LIGHTING );

        // draw axis
        glPushName(x_label);
        glColor4f(1, 0, 0, 1);
        glBegin(GL_LINES);
        glVertex3d(0, 0, 0);
        glVertex3d(m_axisSize, 0, 0);
        glEnd();
        glPopName();

        glPushName(y_label);
        glColor4f(0, 1, 0, 1);
        glBegin(GL_LINES);
        glVertex3d(0, 0, 0);
        glVertex3d(0, m_axisSize, 0);
        glEnd();
        glPopName();

        glPushName(z_label);
        glColor4f(0, 0, 1, 1);
        glBegin(GL_LINES);
        glVertex3d(0, 0, 0);
        glVertex3d(0, 0, m_axisSize);
        glEnd();
        glPopName();

        glPopAttrib();
    }

protected:
    float m_axisSize;
    bool m_rotatable, m_translatable;
    int   x_label, y_label, z_label;
};

} // SceneGraph

#endif // GLMOVABLEAXIS_H
