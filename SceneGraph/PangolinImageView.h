#pragma once

#include <pangolin/pangolin.h>
#include "GLHelpers.h"

namespace SceneGraph
{

//! Specialisation of pangolin::View that automatically sets an orthographic
//! tranforms to display image data and any external draw functions.
class ImageView : public pangolin::View
{
public:
    ImageView(bool flipy = true, bool sampling_linear = true )
        : m_bFlipy(flipy), m_bSamplingLinear(sampling_linear), m_bImageDataDirty(false), m_pImageData(0), m_iImageDataSizeBytes(0)
    {
    }

    ~ImageView() {
        if( m_pImageData) {
            delete [] m_pImageData;
        }
    }

    void ResizeTexture(GLint width, GLint height, GLint internal_format = GL_RGB8)
    {
        this->SetAspect((double)width / (double)height);

        if(m_bFlipy) {
            m_ortho = pangolin::ProjectionMatrixOrthographic(-0.5, width-0.5, height-0.5, -0.5, 0, 1E4 );
        }else{
            m_ortho = pangolin::ProjectionMatrixOrthographic(-0.5, width-0.5, -0.5, height-0.5, 0, 1E4 );
        }

        tex.Reinitialise(width, height, internal_format, m_bSamplingLinear);
    }

    //! Set the image data to be used for display.
    //! The OpenGL Texture associated with this display will be
    //! updated before drawing from within the OpenGL thread.
    void SetImage(const void* pImageData, GLint w, GLint h, GLint internal_format = GL_RGB8, GLenum nFormat = GL_RGB, GLenum nType = GL_UNSIGNED_BYTE)
    {
        const size_t nMemSize = w * h * GLBytesPerPixel( nFormat, nType );

        // Resize buffer if needed
        if(nMemSize > m_iImageDataSizeBytes) {
            if(m_pImageData) {
              delete [] m_pImageData;
            }
            m_pImageData = new unsigned char[nMemSize];
            m_iImageDataSizeBytes = nMemSize;
        }

        // Update Cpu image data
        memcpy( m_pImageData, pImageData, nMemSize );
        m_iImageWidth = w;
        m_iImageHeight = h;
        m_nInternalFormat = internal_format;
        m_nFormat = nFormat;
        m_nType = nType;

        // Set gl texture as dirty
        m_bImageDataDirty = true;
    }

    //! This should only be called from within the OpenGL Thread.
    void UpdateGlTexture()
    {
        if(m_iImageWidth != tex.width || m_iImageHeight != tex.height || m_nInternalFormat != tex.internal_format) {
            ResizeTexture(m_iImageWidth, m_iImageHeight, m_nInternalFormat);
        }

        m_bImageDataDirty = false;
        tex.Upload(m_pImageData, m_nFormat, m_nType);
    }

    //! Render this view.
    virtual void Render()
    {
        // Upload image data to texture if modified
        if(m_bImageDataDirty) {
            UpdateGlTexture();
        }

        // Activate viewport
        this->Activate();
        glColor3f(1,1,1);

        // Load orthographic projection matrix to match image
        glMatrixMode(GL_PROJECTION);
        m_ortho.Load();

        // Reset ModelView matrix
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Render texture
        tex.Bind();
        glEnable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2d(-0.5,-0.5);
        glTexCoord2f(1, 0); glVertex2d(tex.width-0.5,-0.5);
        glTexCoord2f(1, 1); glVertex2d(tex.width-0.5,tex.height-0.5);
        glTexCoord2f(0, 1); glVertex2d(-0.5,tex.height-0.5);
        glEnd();
        glDisable(GL_TEXTURE_2D);

        // Call base View implementation
        pangolin::View::Render();
    }

protected:
    // OpenGL Texture
    pangolin::GlTexture tex;

    // Projection matrix
    pangolin::OpenGlMatrix m_ortho;

    // Should the image be drawn counter to OpenGL Convention
    bool   m_bFlipy;
    bool   m_bSamplingLinear;

    // Is the cached image data newer than the GlTexture data
    bool   m_bImageDataDirty;

    // Saved Image data to be uploaded before draw
    GLint m_iImageWidth;
    GLint m_iImageHeight;
    unsigned char*  m_pImageData;
    size_t m_iImageDataSizeBytes;
    GLint m_nInternalFormat;
    GLenum m_nFormat;
    GLenum m_nType;
};

}
