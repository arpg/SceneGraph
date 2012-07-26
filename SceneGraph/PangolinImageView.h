#pragma once

#include <pangolin/pangolin.h>
#include "GLHelpers.h"

namespace SceneGraph
{

class ImageView : public pangolin::View, public pangolin::GlTexture
{
public:
    ImageView(GLint width, GLint height, GLint internal_format = GL_RGB8, bool flipy = true, bool sampling_linear = true)
        : pangolin::GlTexture(width,height,internal_format, sampling_linear), m_bFlipy(flipy), m_pImageData(0), m_iImageDataSizeBytes(0)
    {
        this->SetAspect((double)width / (double)height);
    }

    ~ImageView() {
        if( m_pImageData) {
            delete [] m_pImageData;
        }
    }

    //! Set the image data to be used for display.
    //! The OpenGL Texture associated with this display will be
    //! updated before drawing from within the OpenGL thread.
    void SetImage(void* pImageData, GLenum nFormat = GL_RGB, GLenum nType = GL_UNSIGNED_BYTE)
    {
        const size_t nMemSize = this->width * this->height * GLBytesPerPixel( nFormat, nType );

        // Resize buffer if needed
        if(nMemSize != m_iImageDataSizeBytes) {
            delete [] m_pImageData;
            m_pImageData = new unsigned char[nMemSize];
            m_iImageDataSizeBytes = nMemSize;
        }

        // Update Cpu image data
        memcpy( m_pImageData, pImageData, nMemSize );
        m_nFormat = nFormat;
        m_nType = nType;

        // Set gl texture as dirty
        m_bImageDataDirty = true;
    }

    //! This should only be called from within the OpenGL Thread.
    void UpdateGlTexture()
    {
        m_bImageDataDirty = false;
        this->Upload(m_pImageData, m_nFormat, m_nType);
    }

    //! Render this view.
    virtual void Render()
    {
        if(m_bImageDataDirty) {
            UpdateGlTexture();
        }
        this->Activate();
        this->RenderToViewport(m_bFlipy);
    }

protected:
    // Should the image be drawn counter to OpenGL Convention
    bool   m_bFlipy;

    // Is the cached image data newer than the GlTexture data
    bool   m_bImageDataDirty;

    // Saved Image data to be uploaded before draw
    unsigned char*  m_pImageData;
    size_t m_iImageDataSizeBytes;
    GLenum m_nFormat;
    GLenum m_nType;
};

}
