#ifndef GLGROUP_H
#define GLGROUP_H

#include <SceneGraph/GLObject.h>

namespace SceneGraph
{

///////////////////////////////////////////////////////////////////////////////
class GLGroup : public GLObject
{
public:

    GLGroup()
        : GLObject("Axis")
    {
        m_bPerceptable = false;
    }

    void DrawCanonicalObject()
    {
        // Do nothing
    }

    /// Include children in calllist (since the group itself does nothing)
    virtual void CompileAsGlCallList()
    {
        if( m_nDisplayList == -1 ){
            m_nDisplayList = glGenLists(1);
            glNewList( m_nDisplayList, GL_COMPILE );
            DrawChildren();
            glEndList();
        }
    }

    /// Override GLObject::DrawObjectAndChildren since children are included
    /// in call list.
    virtual void DrawObjectAndChildren(RenderMode renderMode)
    {
        if( IsVisible() && (
                (renderMode == eRenderVisible) || (renderMode == eRenderNoPrePostHooks) ||
                (renderMode == eRenderSelectable && (IsSelectable() || m_vpChildren.size() > 0) ) ||
                (renderMode == eRenderPerceptable && IsPerceptable())
            )
        ) {
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glMultMatrixd(m_T_po.data());
            glScaled(m_dScale[0],m_dScale[1],m_dScale[2]);

            if(m_nDisplayList >= 0) {
                glCallList( m_nDisplayList );
            }else{
                DrawChildren();
            }

            glPopMatrix();
        }
    }

};

} // SceneGraph

#endif // GLGROUP_H
