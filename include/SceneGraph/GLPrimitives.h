#pragma once

#include <Eigen/StdVector>
#include <SceneGraph/SceneGraph.h>
#include <SceneGraph/PangolinGlCachedSizeableBuffer.h>

namespace SceneGraph
{

typedef Eigen::Matrix<double, 6, 1> Vector6d;

typedef std::vector<Vector6d, Eigen::aligned_allocator<Vector6d> >
Vector6dAlignedVec;

typedef std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> >
Vector3dAlignedVec;

template<typename GlBufferType = pangolin::GlSizeableBuffer>
class GLPrimitives
  : public GLObject
{
public:
    GLPrimitives(GLenum mode = GL_LINE_STRIP, SceneGraph::GLColor color = SceneGraph::GLColor(), int initial_vert_buffer_elements = 1024)
        : GLObject("GLPrimitives"), m_mode(mode), m_color(color),
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
        glLineWidth(5);
        glDrawArrays(m_mode, m_vbo.start(), m_vbo.size() );
        glDisableClientState(GL_VERTEX_ARRAY);
        m_vbo.Unbind();        
    }    
    
    template<typename Derived> inline
    void AddVertex( const Eigen::DenseBase<Derived>& vec)
    {
        GLObject::m_aabb.Insert(vec);
        m_vbo.Add(vec.template cast<float>() );
    }

    void AddVertex(const Eigen::Vector3d& p)
    {
        GLObject::m_aabb.Insert(p);
        m_vbo.Add(p.cast<float>() );
    }

    void AddVertex(const Eigen::Vector3f& p)
    {
        Eigen::Vector3d temp;
        temp = p.cast<double>();
        GLObject::m_aabb.Insert(temp);
        m_vbo.Add( p );
    }

    void AddVerticesFromTrajectory(const Vector6dAlignedVec& vPts) {
      m_vbo.Clear();
      for (size_t i = 0; i < vPts.size(); ++i ) {
        AddVertex(vPts[i][0], vPts[i][1], vPts[i][2]);
      }
    }

    void AddVerticesFromTrajectory(const Vector3dAlignedVec& vPts) {
      m_vbo.Clear();
      for (size_t i = 0; i < vPts.size(); ++i ) {
        Eigen::Vector3f vPt;
        vPt << vPts[i][0], vPts[i][1], vPts[i][2];
        AddVertex(vPt);
      }
    }

    void Clear()
    {
        GLObject::m_aabb.Clear();
        m_vbo.Clear();
    }
    
    void SetColor( const GLColor& c )
    {
        m_color = c;
    }



protected:
    GLenum m_mode;
    SceneGraph::GLColor m_color;
    GlBufferType m_vbo;
};

typedef GLPrimitives<GlCachedSizeableBuffer> GLCachedPrimitives;

}
