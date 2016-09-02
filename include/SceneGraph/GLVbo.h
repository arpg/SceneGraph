#ifndef GLVBO_H
#define GLVBO_H

#include <pangolin/gl/glvbo.h>
#include <SceneGraph/GLObject.h>
#include <SceneGraph/SimCam.h>

namespace SceneGraph
{

///////////////////////////////////////////////////////////////////////////////
class GLVbo : public GLObject
{
public:

    GLVbo(pangolin::GlBuffer* vbo, pangolin::GlBuffer* ibo=0,
          pangolin::GlBuffer* cbo=0, pangolin::GlBuffer* nbo=0)
        : GLObject("Vbo"), m_vbo(vbo), m_ibo(ibo), m_cbo(cbo), m_nbo(nbo)
    {
    }

    void DrawCanonicalObject()
    {
        pangolin::GlState gl;
        
        if(!m_nbo) {
            gl.glDisable(GL_LIGHTING);
        }

        pangolin::RenderVboIboCboNbo(
            *m_vbo,*m_ibo,*m_cbo,*m_nbo,
            m_ibo, m_cbo, m_nbo
        );
    }

protected:
    pangolin::GlBuffer* m_vbo;
    pangolin::GlBuffer* m_ibo;
    pangolin::GlBuffer* m_cbo;
    pangolin::GlBuffer* m_nbo;
};

} // SceneGraph

#endif // GLVBO_H
