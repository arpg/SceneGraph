#pragma once

#include <SceneGraph/SceneGraph.h>
#include <pangolin/gl/gl.h>

namespace SceneGraph
{

/// Public interface mirroring pangolin::GlSizeableBuffer
/// but with cpu cache for delayed batch upload.
class GlCachedSizeableBuffer
    : public pangolin::GlSizeableBuffer
{
public:
    GlCachedSizeableBuffer(pangolin::GlBufferType buffer_type, GLuint initial_num_elements, GLenum datatype, GLuint count_per_element, GLenum gluse = GL_DYNAMIC_DRAW )
        : pangolin::GlSizeableBuffer(buffer_type, initial_num_elements, datatype, count_per_element, gluse),
          m_dirty(false),
          m_num_verts(0),
          m_vs(count_per_element, initial_num_elements)
          
    {
        
    }

    /// Clear cache
    inline void Clear() {
        m_num_verts = 0;
        pangolin::GlSizeableBuffer::Clear();
    }

    /// Add vertices to cpu cache
    template<typename Derived> inline
    void Add(const Eigen::DenseBase<Derived>& vec)
    {
        assert(vec.rows() == (int)pangolin::GlSizeableBuffer::count_per_element);
        CheckResize(m_num_verts + vec.cols() );
        m_vs.block(0,m_num_verts,vec.rows(),vec.cols()) = vec;
        m_num_verts += vec.cols();
        m_dirty = true;
    }
    
    template<typename Derived>
    void Update(const Eigen::DenseBase<Derived>& vec, size_t position = 0)
    {
        assert(vec.rows() == pangolin::GlSizeableBuffer::count_per_element);
        CheckResize(position + vec.cols());
        m_vs.block(0,position, vec.rows(), vec.cols()) = vec;
        m_num_verts = std::max(position+vec.cols(), m_num_verts);        
        m_dirty = true;
    }
    
    /// Force sync of cpu cache to gpu buffer
    inline void Sync()
    {
        if(m_dirty) {
            pangolin::GlSizeableBuffer::Update( m_vs.leftCols(m_num_verts) );
            m_dirty = false;
        }        
    }
    
    /// Sync cache with gpu buffer and bind
    inline void Bind()
    {
        Sync();
        pangolin::GlSizeableBuffer::Bind();
    }
    
    size_t start() const
    {
        return 0;
    }
    
    size_t size() const
    {
        return m_num_verts;
    }    
            
protected:    
    inline void CheckResize(int num_verts)
    {
        if( num_verts > m_vs.cols()) {
            const size_t new_size = NextSize(num_verts);
            Eigen::MatrixXf tmp(pangolin::GlSizeableBuffer::count_per_element, new_size);
            tmp.leftCols(m_num_verts) = m_vs.leftCols(m_num_verts);
            std::swap(m_vs, tmp);
        }
    }
    
    inline size_t NextSize(size_t min_size) const
    {
        size_t new_size = m_vs.cols();
        while(new_size < min_size) {
            new_size *= 2;
        }
        return new_size;
    }    
    
    bool m_dirty;
    int m_num_verts;    
    Eigen::MatrixXf m_vs;
};

}

