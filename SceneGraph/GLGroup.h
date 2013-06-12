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
        : GLObject("Group")
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
#ifndef HAVE_GLES
        if( m_nDisplayList == -1 ){
            m_nDisplayList = glGenLists(1);
            glNewList( m_nDisplayList, GL_COMPILE );
            DrawChildren();
            glEndList();
        }
#endif
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
            
#ifdef HAVE_GLES
            Eigen::Matrix4f T_po = m_T_po.cast<float>();
            glMultMatrixf(T_po.data());
            glScalef(m_dScale[0],m_dScale[1],m_dScale[2]);
            DrawCanonicalObject();
#else
            glMultMatrixd(m_T_po.data());
            glScaled(m_dScale[0],m_dScale[1],m_dScale[2]);
            if(m_nDisplayList >= 0) {
                glCallList( m_nDisplayList );
            }else{
                DrawChildren();
            }
#endif //HAVE_GLES

            glPopMatrix();
        }
    }

};

} // SceneGraph

#endif // GLGROUP_H
