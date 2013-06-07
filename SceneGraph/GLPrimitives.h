#pragma once

#include <SceneGraph/SceneGraph.h>
#include <pangolin/gl.h>

namespace SceneGraph
{

template<typename GlBufferType = pangolin::GlSizeableBuffer>
class GLPrimitives
  : public GLObject
{
public:
    GLPrimitives(GLenum mode = GL_LINE_STRIP, SceneGraph::GLColor color = SceneGraph::GLColor(), int initial_vert_buffer_elements = 1024)
        : m_mode(mode), m_color(color),
          m_vbo(pangolin::GlArrayBuffer, initial_vert_buffer_elements, GL_FLOAT, 3, GL_DYNAMIC_DRAW)
    {
        
    }

    GLPrimitives(const std::vector<Eigen::Vector3d,Eigen::aligned_allocator<Eigen::Vector3d> >& vArray, GLenum mode = GL_LINE_STRIP, SceneGraph::GLColor color = SceneGraph::GLColor())
        : m_mode(mode), m_color(color),
          m_vbo(pangolin::GlArrayBuffer, vArray.size()*3, GL_FLOAT, 3, GL_DYNAMIC_DRAW)
    {
        for( int ii = 0; ii < vArray.size() ; ++ii ){
            AddVertex(vArray[ii]);
        }
    }
    
    void DrawCanonicalObject()
    {        
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
        GLObject::m_aabb.Insert(p);
        m_vbo.Add(p.cast<float>() );
    }
    
    void Clear()
    {
        GLObject::m_aabb.Clear();
        m_vbo.Clear();
    }
    
protected:
    GLenum m_mode;
    SceneGraph::GLColor m_color;
    GlBufferType m_vbo;
};

typedef GLPrimitives<GlCachedSizeableBuffer> GLCachedPrimitives;

}
