#ifndef GLVBO_H
#define GLVBO_H

#include <SceneGraph/GLObject.h>
#include <pangolin/glvbo.h>
#include <SceneGraph/SimCam.h>

namespace SceneGraph
{

///////////////////////////////////////////////////////////////////////////////
class GLVbo : public GLObject
{
public:

    GLVbo(pangolin::GlBuffer* vbo, pangolin::GlBuffer* ibo=0,
          pangolin::GlBuffer* cbo=0, pangolin::GlBuffer* nbo=0,
          Eigen::Matrix4d objectOrigin=Eigen::Matrix4d::Identity())
        : GLObject("Vbo"), m_vbo(vbo), m_ibo(ibo), m_cbo(cbo), m_nbo(nbo),
          m_ObjectOrigin(objectOrigin)
    {
    }

    void DrawCanonicalObject()
    {
        pangolin::GlState gl;
        
        if(!m_nbo) {
            gl.glDisable(GL_LIGHTING);
        }

        glMultMatrix(m_ObjectOrigin);
        pangolin::RenderVboIboCboNbo(
            *m_vbo,*m_ibo,*m_cbo,*m_nbo,
            m_ibo, m_cbo, m_nbo
        );
    }

    void SetObjectOrigin(Eigen::Matrix4d &newObjectOrigin)
    {
      m_ObjectOrigin = newObjectOrigin;
    }

protected:
    pangolin::GlBuffer* m_vbo;
    pangolin::GlBuffer* m_ibo;
    pangolin::GlBuffer* m_cbo;
    pangolin::GlBuffer* m_nbo;

    Eigen::Matrix4d     m_ObjectOrigin;
};

} // SceneGraph

#endif // GLVBO_H
