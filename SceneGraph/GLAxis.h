#ifndef GLAXIS_H
#define GLAXIS_H

#include <SceneGraph/GLObject.h>

namespace SceneGraph
{

///////////////////////////////////////////////////////////////////////////////
class GLAxis : public GLObject
{
public:

    GLAxis(float axisSize = 1.0f)
        : GLObject("Axis"), m_axisSize(axisSize)
    {
        m_bPerceptable = false;
        m_aabb.SetZero();
    }

    static inline void DrawAxis(float size = 1.0f)
    {
        // draw axis
        glBegin(GL_LINES);
        glColor4f(1, 0, 0, 1);
        glVertex3d(0, 0, 0);
        glVertex3d(size, 0, 0);

        glColor4f(0, 1, 0, 1);
        glVertex3d(0, 0, 0);
        glVertex3d(0, size, 0);

        glColor4f(0, 0, 1, 1);
        glVertex3d(0, 0, 0);
        glVertex3d(0, 0, size);
        glEnd();
    }

    void DrawCanonicalObject()
    {
        DrawAxis(m_axisSize);
    }

    void SetAxisSize(float size){ m_axisSize = size; }
    float GetAxisSize() { return m_axisSize; }

protected:
    float m_axisSize;
};

} // SceneGraph

#endif // GLAXIS_H
