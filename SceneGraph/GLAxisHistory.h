#pragma once

#include <SceneGraph/SceneGraph.h>
#include <pangolin/gl.h>

namespace SceneGraph
{

class GLAxisHistory
  : public GLObject
{
public:
    typedef Eigen::Matrix<float,3,1,Eigen::DontAlign> float3;
    
    GLAxisHistory(int initial_vert_buffer_size = 1024)
        : m_red(pangolin::GlArrayBuffer, initial_vert_buffer_size, GL_FLOAT, 3, GL_DYNAMIC_DRAW),
          m_green(pangolin::GlArrayBuffer, initial_vert_buffer_size, GL_FLOAT, 3, GL_DYNAMIC_DRAW),
          m_blue(pangolin::GlArrayBuffer, initial_vert_buffer_size, GL_FLOAT, 3, GL_DYNAMIC_DRAW)          
    {
    }
    
    ~GLAxisHistory()
    {
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
    
//#ifdef HAVE_SOPHUS
//    void AddAxis(const Sophus::SE3d& T_wp, double axis_size = 1.0)
//    {
//        Eigen::Matrix<float,3,2> vs;
        
//        vs.block<3,1>(0,0) = T_wp.translation().cast<float>();
//        vs.block<3,1>(0,1) = (T_wp * Eigen::Vector3d(axis_size,0,0)).cast<float>();
//        m_red.Add(vs);

//        vs.block<3,1>(0,1) = (T_wp * Eigen::Vector3d(0,axis_size,0)).cast<float>();
//        m_green.Add(vs);
        
//        vs.block<3,1>(0,1) = (T_wp * Eigen::Vector3d(0,0,axis_size)).cast<float>();
//        m_blue.Add(vs);
//    }
//#endif
    
    void AddAxis(const Eigen::Matrix<double,3,4>& T_wp, double axis_size = 1.0)
    {
        Eigen::Matrix<float,3,2> vs;
        
        vs.block<3,1>(0,0) = T_wp.block<3,1>(0,3).cast<float>();
        vs.block<3,1>(0,1) = (T_wp * Eigen::Vector4d(axis_size,0,0,1)).cast<float>();
        m_red.Add(vs);

        vs.block<3,1>(0,1) = (T_wp * Eigen::Vector4d(0,axis_size,0,1)).cast<float>();
        m_green.Add(vs);
        
        vs.block<3,1>(0,1) = (T_wp * Eigen::Vector4d(0,0,axis_size,1)).cast<float>();
        m_blue.Add(vs);
    }
    
protected:
    void DrawVbo(pangolin::GlSizeableBuffer& vbo)
    {
        vbo.Bind();
        glVertexPointer(vbo.count_per_element, vbo.datatype, 0, 0);
        glEnableClientState(GL_VERTEX_ARRAY);
    
        glDrawArrays(GL_LINES, vbo.start(), vbo.size() );
    
        glDisableClientState(GL_VERTEX_ARRAY);
        vbo.Unbind();        
    }
    
    pangolin::GlSizeableBuffer m_red;
    pangolin::GlSizeableBuffer m_green;
    pangolin::GlSizeableBuffer m_blue;
};

}
