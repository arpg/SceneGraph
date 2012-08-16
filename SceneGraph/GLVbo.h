#ifndef GLVBO_H
#define GLVBO_H

#include <SceneGraph/GLObject.h>
#include <pangolin/glvbo.h>

namespace SceneGraph
{

///////////////////////////////////////////////////////////////////////////////
class GLVbo : public GLObject
{
public:

    GLVbo(pangolin::GlBuffer* vbo, pangolin::GlBuffer* ibo=0, pangolin::GlBuffer* cbo=0, pangolin::GlBuffer* nbo=0)
        : GLObject("Vbo"), m_vbo(vbo), m_ibo(ibo), m_cbo(cbo), m_nbo(nbo)
    {
    }

    void DrawCanonicalObject()
    {
        RenderVboIboCboNbo(
            *m_vbo,*m_ibo,*m_cbo,*m_nbo,
            m_vbo->width, m_vbo->height,
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
