#ifndef GLAXIS_H
#define GLAXIS_H

#include <SceneGraph/GLObject.h>

namespace SceneGraph
{

///////////////////////////////////////////////////////////////////////////////
class GLAxis : public GLObject
{
public:

    GLAxis()
      : GLObject("Axis")
    {
        m_bPerceptable = false;
    }

    static void DrawUnitAxis()
    {
        // draw axis
        glBegin(GL_LINES);
        glColor4f(1, 0, 0, 1);
        glVertex3d(0, 0, 0);
        glVertex3d(1, 0, 0);

        glColor4f(0, 1, 0, 1);
        glVertex3d(0, 0, 0);
        glVertex3d(0, 1, 0);

        glColor4f(0, 0, 1, 1);
        glVertex3d(0, 0, 0);
        glVertex3d(0, 0, 1);
        glEnd();
    }

    void DrawCanonicalObject()
    {
        glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT);
        glDisable( GL_LIGHTING );
        DrawUnitAxis();
        glPopAttrib();
    }
};

} // SceneGraph

#endif // GLAXIS_H
