#pragma once

#include <SceneGraph/SceneGraph.h>

namespace SceneGraph
{

class GLPrimitives
  : public GLObject
{
public:
    GLPrimitives(GLenum mode = GL_LINE_STRIP, SceneGraph::GLColor color = SceneGraph::GLColor(), int initial_vert_buffer_size = 1024)
        : m_mode(mode), m_color(color),
          m_vbo(pangolin::GlArrayBuffer, initial_vert_buffer_size, GL_FLOAT, 3, GL_DYNAMIC_DRAW)
    {
        
    }
    
    void DrawCanonicalObject()
    {        
        m_color.Apply();
        
        m_vbo.Bind();
        glVertexPointer(m_vbo.count_per_element, m_vbo.datatype, 0, 0);
        glEnableClientState(GL_VERTEX_ARRAY);
        glDrawArrays(GL_LINES, m_vbo.start(), m_vbo.size() );
        glDisableClientState(GL_VERTEX_ARRAY);
        m_vbo.Unbind();        
    }    
    
    void AddVertex(Eigen::Vector3d p)
    {
        m_vbo.Add(p);
    }
    
protected:
    GLenum m_mode;
    SceneGraph::GLColor m_color;
    pangolin::GlSizeableBuffer m_vbo;
};

}
