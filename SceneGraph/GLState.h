#pragma once

#ifndef GLSTATE_H
#define GLSTATE_H

class GLState {

    class CapabilityEnabled {
    public:

        CapabilityEnabled(GLenum cap, GLboolean enable)
            : m_cap(cap), m_enable(enable)
        {

        }

        void Apply() {
            if(m_enable) {
                ::glEnable(m_cap);
            }else{
                ::glDisable(m_cap);
            }
        }

        void UnApply() {
            if(m_enable) {
                ::glDisable(m_cap);
            }else{
                ::glEnable(m_cap);
            }
        }

    protected:
        GLenum m_cap;
        GLboolean m_enable;
    };

    ~GLState() {
        //  Restore original state
        while (!m_history.empty()) {
            m_history.top().UnApply();
            m_history.pop();
        }

        if (m_DepthMaskCalled) {
            ::glDepthMask(m_OriginalDepthMask);
        }

        if (m_ShadeModelCalled) {
            ::glShadeModel(m_OriginalShadeModel);
        }

        if (m_ColorMaskCalled) {
            ::glColorMask(m_OriginalColorMask[0], m_OriginalColorMask[1], m_OriginalColorMask[2], m_OriginalColorMask[3]);
        }

        if (m_ViewportCalled) {
            ::glViewport(m_OriginalViewport[0], m_OriginalViewport[1], m_OriginalViewport[2], m_OriginalViewport[3]);
        }
    }

    static inline GLboolean EnableValue(GLenum cap)
    {
        GLboolean curVal;
        glGetBooleanv(cap, &curVal);
        return curVal;
    }

    inline void glEnable(GLenum cap)
    {
        if(!EnableValue(cap)) {
            m_history.push(CapabilityEnabled(cap,false));
            glEnable(cap);
        }
    }


    inline void glDisable(GLenum cap)
    {
        if(EnableValue(cap)) {
            m_history.push(CapabilityEnabled(cap,true));
            glDisable(cap);
        }
    }

    GLboolean m_DepthMaskCalled;
    GLboolean m_OriginalDepthMask;
    inline void glDepthMask(GLboolean flag)
    {
        m_DepthMaskCalled = true;
        glDepthMask(flag);
        glGetBooleanv(GL_DEPTH_WRITEMASK, &m_OriginalDepthMask);
    }

    GLboolean m_ShadeModelCalled;
    GLboolean m_OriginalShadeModel;
    inline void glShadeModel(GLenum mode)
    {
        glGetBooleanv(GL_SHADE_MODEL, &m_OriginalShadeModel);
        glShadeModel(mode);
    }

    GLboolean m_ColorMaskCalled;
    GLboolean m_OriginalColorMask[4];
    inline void glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
    {
        glGetBooleanv(GL_COLOR_WRITEMASK,  m_OriginalColorMask);
        glColorMask(red, green, blue, alpha);
    }

    GLboolean m_ViewportCalled;
    GLint m_OriginalViewport[4];
    inline void glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
    {
        glGetIntegerv(GL_VIEWPORT, m_OriginalViewport);
        gluViewport(x, y, width, height);
    }


    std::stack<CapabilityEnabled> m_history;

};


#endif // GLSTATE_H
