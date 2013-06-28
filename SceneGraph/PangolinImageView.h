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
    ImageView(bool flipy = true, bool sampling_linear = true, bool use_image_aspect = true )
        : m_bFlipy(flipy), 
          m_bSamplingLinear(sampling_linear), 
          m_bImageDataDirty(false), 
          m_pImageData(0), 
          m_iImageDataSizeBytes(0),
          m_bUseImageAspect(use_image_aspect)
    {
    }

    ~ImageView() {
        if( m_pImageData) {
            delete [] m_pImageData;
        }
    }

    void ResizeTexture(GLint width, GLint height, GLint internal_format, GLint glformat, GLint gltype)
    {
        if(m_bUseImageAspect) {
            this->SetAspect((double)width / (double)height);
        }

        if(m_bFlipy) {
            m_ortho = pangolin::ProjectionMatrixOrthographic(-0.5, width-0.5, height-0.5, -0.5, 0, 1E4 );
        }else{
            m_ortho = pangolin::ProjectionMatrixOrthographic(-0.5, width-0.5, -0.5, height-0.5, 0, 1E4 );
        }

        tex.Reinitialise(width, height, internal_format, m_bSamplingLinear, 0, glformat, gltype);
    }

    //! Set the image data to be used for display.
    //! The OpenGL Texture associated with this display will be
    //! updated before drawing from within the OpenGL thread.
    void SetImage(const void* pImageData, GLint w, GLint h, GLint internal_format = GL_RGB, GLenum nFormat = GL_RGB, GLenum nType = GL_UNSIGNED_BYTE)
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

    //! Get pointer to image data
    unsigned char* Data()
    {
        return m_pImageData;
    }
    //! This should only be called from within the OpenGL Thread.
    void UpdateGlTexture()
    {
        if(m_iImageWidth != tex.width || m_iImageHeight != tex.height || m_nInternalFormat != tex.internal_format) {
            ResizeTexture(m_iImageWidth, m_iImageHeight, m_nInternalFormat, m_nFormat, m_nType);
        }

        m_bImageDataDirty = false;
        tex.Upload(m_pImageData, m_nFormat, m_nType);
    }

    void SetSamplingLinear(bool sampling_linear)
    {
        m_bSamplingLinear = sampling_linear;
    }

    //! Render this view.
    virtual void Render()
    {
        // Upload image data to texture if modified
        if(m_bImageDataDirty) {
            UpdateGlTexture();
        }

        glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_LIGHTING);
        glDisable(GL_COLOR_MATERIAL);
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);

        // Activate viewport
        pangolin::Viewport::DisableScissor();
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
        
        const GLfloat sq_vert[] = {
            -0.5f, -0.5f,
            -0.5f, (float)tex.height-0.5f,
            (float)tex.width-0.5f, (float)tex.height-0.5f,  
            (float)tex.width-0.5f, -0.5f
        };
        glVertexPointer(2, GL_FLOAT, 0, sq_vert);
        glEnableClientState(GL_VERTEX_ARRAY);   
    
        const GLfloat sq_tex[]  = { 0,0,  0,1,  1,1,  1,0  }; // normal
//        const GLfloat sq_tex[]  = { 0,1,  0,0,  1,0,  1,1  }; // flipped
        glTexCoordPointer(2, GL_FLOAT, 0, sq_tex);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
             
        glColor4f(1,1,1,1);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    
        glDisable(GL_TEXTURE_2D);

        // Call base View implementation
        pangolin::View::Render();

        glPopAttrib();
    }

    void ImageToWindowCoords(const Eigen::Vector2d& p_pix, Eigen::Vector2d& p_win)
    {
        p_win(0) = v.l + v.w * p_pix(0) / m_iImageWidth;
        p_win(1) = v.b + v.h * (float)(m_iImageHeight - p_pix(1)) / m_iImageHeight;
    }

    void WindowToImageCoords(const Eigen::Vector2d& p_win, Eigen::Vector2d& p_pix)
    {
        p_pix(0) =  m_iImageWidth * (p_win(0) - v.l)/v.w ;
        p_pix(1) = m_iImageHeight - m_iImageHeight* (p_win(1) - v.b) / v.h;
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
    bool m_bUseImageAspect;
    GLint m_nInternalFormat;
    GLenum m_nFormat;
    GLenum m_nType;
};

}
