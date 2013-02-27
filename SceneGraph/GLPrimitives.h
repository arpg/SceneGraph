#pragma once

#include <SceneGraph/SceneGraph.h>
#include <pangolin/gl.h>

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
    
    void Clear()
    {
        m_vbo.Clear();
    }
    
protected:
    GLenum m_mode;
    SceneGraph::GLColor m_color;
    pangolin::GlSizeableBuffer m_vbo;
};

class GLCachedPrimitives
  : public GLObject
{
public:
    GLCachedPrimitives(GLenum mode = GL_LINE_STRIP, SceneGraph::GLColor color = SceneGraph::GLColor(), int initial_vert_buffer_size = 1024)
        : m_mode(mode), m_color(color),
          m_dirty(false),
          m_num_verts(0),
          m_vs(3, initial_vert_buffer_size),
          m_vbo(pangolin::GlArrayBuffer, initial_vert_buffer_size, GL_FLOAT, 3, GL_DYNAMIC_DRAW)
    {
        
    }
    
    void DrawCanonicalObject()
    {        
        if(m_dirty) {
            m_vbo.Update(m_vs.block(0,0,3,m_num_verts));
        }
        
        m_color.Apply();
        
        m_vbo.Bind();
        glVertexPointer(m_vbo.count_per_element, m_vbo.datatype, 0, 0);
        glEnableClientState(GL_VERTEX_ARRAY);
        glDrawArrays(m_mode, m_vbo.start(), m_vbo.size() );
        glDisableClientState(GL_VERTEX_ARRAY);
        m_vbo.Unbind();        
    }    
    
    void AddVertex(Eigen::Vector3d p)
    {
        CheckResize(m_num_verts+1);
        m_vs.col(m_num_verts) = p.cast<float>();
        ++m_num_verts;
        m_dirty = true;
    }
    
    void Clear()
    {
        m_num_verts = 0;
        m_vbo.Clear();
    }    
    
protected:
    void CheckResize(int num_verts)
    {
        if( num_verts > m_vs.cols()) {
            const size_t new_size = NextSize(num_verts);
            Eigen::Matrix<float,3,Eigen::Dynamic> tmp(3, new_size);
            tmp.block(0,0,3,m_num_verts) = m_vs.block(0,0,3,m_num_verts);
            std::swap(m_vs, tmp);
        }
    }
    
    size_t NextSize(size_t min_size) const
    {
        size_t new_size = m_vs.cols();
        while(new_size < min_size) {
            new_size *= 2;
        }
        return new_size;
    }    
    
    bool m_dirty;
    int m_num_verts;
    GLenum m_mode;
    SceneGraph::GLColor m_color;
    Eigen::Matrix<float, 3, Eigen::Dynamic> m_vs;
    pangolin::GlSizeableBuffer m_vbo;
};

}
