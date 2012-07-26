#pragma once

#include <pangolin/pangolin.h>
#include "GLHelpers.h"

namespace SceneGraph
{

//! Specialisation of pangolin::View that automatically sets an orthographic
//! tranforms to display image data and any external draw functions.
class ImageView : public pangolin::View, public pangolin::GlTexture
{
public:
    ImageView(GLint width, GLint height, GLint internal_format = GL_RGB8, bool flipy = true, bool sampling_linear = true)
        : pangolin::GlTexture(width,height,internal_format, sampling_linear), m_bFlipy(flipy), m_pImageData(0), m_iImageDataSizeBytes(0)
    {
        this->SetAspect((double)width / (double)height);

        if(m_bFlipy) {
            m_ortho = pangolin::ProjectionMatrixOrthographic(-0.5, width-0.5, height-0.5, -0.5, 0, 1E4 );
        }else{
            m_ortho = pangolin::ProjectionMatrixOrthographic(-0.5, width-0.5, -0.5, height-0.5, 0, 1E4 );
        }

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
        Bind();
        glEnable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2d(-0.5,-0.5);
        glTexCoord2f(1, 0); glVertex2d(width-0.5,-0.5);
        glTexCoord2f(1, 1); glVertex2d(width-0.5,height-0.5);
        glTexCoord2f(0, 1); glVertex2d(-0.5,height-0.5);
        glEnd();
        glDisable(GL_TEXTURE_2D);

        // Call base View implementation
        pangolin::View::Render();
    }

protected:
    // Projection matrix
    pangolin::OpenGlMatrix m_ortho;

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
