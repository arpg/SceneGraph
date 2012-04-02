#ifndef _FBO_H_
#define _FBO_H_

#include <SimpleGui/GLHelpers.h>

#include <vector>
#include <iostream>

class FBO
{
    public:
        ///////////////////////////////////////////////////////////////////////////////
        static FBO* Instance();
	    
        ///////////////////////////////////////////////////////////////////////////////
        ~FBO()
        {
            glDeleteTextures( m_vColorTextureIds.size(), &m_vColorTextureIds[0] );
            glDeleteTextures(1, &m_nDepthTextureId );
            glDeleteFramebuffersEXT( 1, &m_nFrameBufferId );
        }

        ///////////////////////////////////////////////////////////////////////////////
        void BindTexture( unsigned int nIndex )
        {
            glEnable( GL_TEXTURE_RECTANGLE_ARB );
            glBindTexture( GL_TEXTURE_RECTANGLE_ARB, m_vColorTextureIds[nIndex] );
        }

        ///////////////////////////////////////////////////////////////////////////////
        void BindDepthTexture()
        {
            glEnable( GL_TEXTURE_RECTANGLE_ARB );
            glBindTexture( GL_TEXTURE_RECTANGLE_ARB, m_nDepthTextureId  );
        }

        ///////////////////////////////////////////////////////////////////////////////
        void UnbindTexture()
        {
            glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
        }

        ///////////////////////////////////////////////////////////////////////////////
        unsigned int TexWidth()
        {
            return m_nTexWidth;
        }

        ///////////////////////////////////////////////////////////////////////////////
        unsigned int TexHeight()
        {
            return m_nTexHeight;
        }

        ///////////////////////////////////////////////////////////////////////////////
        /// Change rtt texture widh -- will not re-allocate mem if possible
        void SetTexWidth(
                unsigned int nTexWidth
                )
        {
            if( nTexWidth > 0 && nTexWidth*m_nTexHeight <  m_nInitialTexWidth*m_nInitialTexHeight ){
                m_nTexWidth = nTexWidth;
            }
            else{
                printf("DEBUG: not applying texture width %d -- too big!\n", nTexWidth );
            }
        }

        ///////////////////////////////////////////////////////////////////////////////
        /// Change rtt texture height -- will not re-allocate mem if possible
        void SetTexHeight(
                unsigned int nTexHeight
                )
        {
            if( nTexHeight > 0 && m_nTexWidth*nTexHeight <  m_nInitialTexWidth*m_nInitialTexHeight ){
                m_nTexHeight = nTexHeight;
            }
            else{
                printf("DEBUG: not applying texture height %d -- too big!\n", nTexHeight );
            }
         }

        ///////////////////////////////////////////////////////////////////////////////
        // User should alread have called glDrawBuffer so select the fbo attachment...
        void Begin()
        {
            // adjust viewport and projection matrix to texture dimension
            glGetIntegerv( GL_VIEWPORT,  &m_vViewport[0] );
            glViewport( 0, 0, m_nTexWidth, m_nTexHeight );
            // set the rendering destination to FBO
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_nFrameBufferId );
            glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, m_nRenderBufferId );
            CheckForGLErrors();
        }

        ///////////////////////////////////////////////////////////////////////////////
        void End()
        {
            // back to normal window-system-provided framebuffer
            glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 ); // unbind
            glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, 0 );

            // trigger mipmaps generation explicitly
            // NOTE: If GL_GENERATE_MIPMAP is set to GL_TRUE, then glCopyTexSubImage2D()
            // triggers mipmap generation automatically. However, the texture attached
            // onto a FBO should generate mipmaps manually via glGenerateMipmapEXT().
//            glBindTexture(GL_TEXTURE_RECTANGLE_ARB, m_vColorTextureIds[0] );
//            glGenerateMipmapEXT( GL_TEXTURE_RECTANGLE_ARB );
//            glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);

            // rendering as normal
            // back to normal viewport and projection matrix
            glViewport( m_vViewport[0], m_vViewport[1], m_vViewport[2], m_vViewport[3] );
        }

        ///////////////////////////////////////////////////////////////////////////////
        void Init( unsigned int nTexWidth, unsigned int nTexHeight )
        {
            m_nTexWidth  = nTexWidth;
            m_nTexHeight = nTexHeight;
            m_nInitialTexWidth  = 2*nTexWidth;
            m_nInitialTexHeight = 2*nTexHeight;


            glGetIntegerv( GL_VIEWPORT,  &m_vViewport[0] );

            // Generate color textures  
            glGenTextures( m_vColorTextureIds.size(), &m_vColorTextureIds[0] ); 
            for( size_t ii = 0; ii < m_vColorTextureIds.size(); ii++ ){
                // Bind the texture 
                glBindTexture( GL_TEXTURE_RECTANGLE_ARB, m_vColorTextureIds[ii] ); 

                // Create a standard texture with the width and height of our window  
                glTexImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, m_nTexWidth, 
                        m_nTexHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
                CheckForGLErrors();

                // Setup the basic texture parameters  
//                glTexParameterf( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
                glTexParameterf( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
                glTexParameterf( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
                glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
                glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
            }
            CheckForGLErrors();

            // Generate one depth texture
            {
                glGenTextures( 1, &m_nDepthTextureId );
                // Bind the texture
                glBindTexture( GL_TEXTURE_RECTANGLE_ARB, m_nDepthTextureId );

                /*
                   glTexParameterf( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                   glTexParameterf( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                   glTexParameterf( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
                   glTexParameterf( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
                   glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE );

                 */
                glTexImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, m_nTexWidth, 
                        m_nTexHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );

                //  Unbind the texture  
                glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);  
            }
            CheckForGLErrors();


            // create a framebuffer object, you need to delete them when program exits.
            glGenFramebuffersEXT( 1, &m_nFrameBufferId );
            glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_nFrameBufferId );

            // create a renderbuffer object to store depth info
            // NOTE: A depth renderable image should be attached the FBO for depth test.
            // If we don't attach a depth renderable image to the FBO, then
            // the rendering output will be corrupted because of missing depth test.
            // If you also need stencil test for your rendering, then you must
            // attach additional image to the stencil attachement point, too.
            glGenRenderbuffersEXT( 1, &m_nRenderBufferId );
            glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, m_nRenderBufferId );
            glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, m_nTexWidth, m_nTexHeight );
            glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, 0 );

            // attach a texture to FBO color attachement point
            for( size_t ii = 0; ii < m_vColorTextureIds.size(); ii++ ){
                glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT+ii, GL_TEXTURE_RECTANGLE_ARB,  m_vColorTextureIds[ii], 0);
                }

            // attach a renderbuffer to depth attachment point. wo this depth is is broken...
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_nRenderBufferId );

            // disable color buffer if you don't attach any color buffer image,
            // for example, rendering depth buffer only to a texture.
            // Otherwise, glCheckFramebufferStatusEXT will not be complete.
            // glDrawBuffer(GL_NONE);
            // glReadBuffer(GL_NONE);

            // check FBO status
            CheckFBOStatus();
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
            CheckForGLErrors();
        }

        GLuint                     m_nFrameBufferId;   // ID of FBO
        GLuint                     m_nRenderBufferId;  // ID of Renderbuffer object
        GLuint                     m_nDepthRenderBufferId;
        GLuint                     m_nDepthTextureId;
        std::vector<GLuint>        m_vColorTextureIds;
        GLint                      m_vViewport[4];
        unsigned int               m_nTexWidth;
        unsigned int               m_nTexHeight;
        unsigned int               m_nInitialTexWidth;
        unsigned int               m_nInitialTexHeight;

    private:

        ///////////////////////////////////////////////////////////////////////////////
	FBO(){
	    m_vColorTextureIds.resize(8); // should lookup max number...
	}        
	static FBO*                m_pInstance;
};

#endif

