#pragma once

#include <pangolin/gl/gl.h>
#include <SceneGraph/SceneGraph.h>

namespace SceneGraph
{

template<typename GlBufferType = pangolin::GlSizeableBuffer>
class GLAxisHistory
  : public GLObject
{
public:
    GLAxisHistory(int initial_vert_buffer_size = 1024)
        : m_red(pangolin::GlArrayBuffer, initial_vert_buffer_size, GL_FLOAT, 3, GL_DYNAMIC_DRAW),
          m_green(pangolin::GlArrayBuffer, initial_vert_buffer_size, GL_FLOAT, 3, GL_DYNAMIC_DRAW),
          m_blue(pangolin::GlArrayBuffer, initial_vert_buffer_size, GL_FLOAT, 3, GL_DYNAMIC_DRAW)          
    {
    }
    
    ~GLAxisHistory()
    {
    }
    
    void Clear()
    {
        GLObject::m_aabb.Clear();        
        m_red.Clear();
        m_green.Clear();
        m_blue.Clear();
    }
    
    void DrawCanonicalObject()
    {        
        glColor3f(1,0,0);
        DrawVbo(m_red);
        
        glColor3f(0,1,0);
        DrawVbo(m_green);
        
        glColor3f(0,0,1);
        DrawVbo(m_blue);
    }
    
    int Size() const
    {
        return m_red.size() / 2;
    }    
        
    void AddAxis(const Eigen::Matrix<double,3,4>& T_wp, double axis_size = 1.0)
    {
        GLObject::m_aabb.Insert(T_wp.block<3,1>(0,3));
        
        Eigen::Matrix<float,3,2> vs;
        
        vs.block<3,1>(0,0) = T_wp.block<3,1>(0,3).cast<float>();
        vs.block<3,1>(0,1) = (T_wp * Eigen::Vector4d(axis_size,0,0,1)).cast<float>();
        m_red.Add(vs);

        vs.block<3,1>(0,1) = (T_wp * Eigen::Vector4d(0,axis_size,0,1)).cast<float>();
        m_green.Add(vs);
        
        vs.block<3,1>(0,1) = (T_wp * Eigen::Vector4d(0,0,axis_size,1)).cast<float>();
        m_blue.Add(vs);
    }
    
    void UpdateAxis(int index, const Eigen::Matrix<double,3,4>& T_wp, double axis_size = 1.0)
    {
        Eigen::Matrix<float,3,2> vs;
        
        vs.block<3,1>(0,0) = T_wp.block<3,1>(0,3).cast<float>();
        vs.block<3,1>(0,1) = (T_wp * Eigen::Vector4d(axis_size,0,0,1)).cast<float>();
        m_red.Update(vs, 2*index);

        vs.block<3,1>(0,1) = (T_wp * Eigen::Vector4d(0,axis_size,0,1)).cast<float>();
        m_green.Update(vs, 2*index);
        
        vs.block<3,1>(0,1) = (T_wp * Eigen::Vector4d(0,0,axis_size,1)).cast<float>();
        m_blue.Update(vs, 2*index);
    }
    
protected:
    void DrawVbo(GlBufferType& vbo)
    {
        vbo.Bind();
        glVertexPointer(vbo.count_per_element, vbo.datatype, 0, 0);
        glEnableClientState(GL_VERTEX_ARRAY);
    
        glDrawArrays(GL_LINES, vbo.start(), vbo.size() );
    
        glDisableClientState(GL_VERTEX_ARRAY);
        vbo.Unbind();        
    }
    
    GlBufferType m_red;
    GlBufferType m_green;
    GlBufferType m_blue;
};

typedef GLAxisHistory<GlCachedSizeableBuffer> GLCachedAxisHistory;


}
