#ifndef _GL_IMAGE_
#define _GL_IMAGE_

#include <SceneGraph/GLObject.h>

namespace SceneGraph
{

class GLImage : public GLObject
{
    public:

		// Construct GLImage from OpenCV image.
        GLImage()
		{
            m_nTex = 0;
            m_pImageData = 0;
            m_bTextureDataChanged = false;
            m_bTextureInitialized = false;
            InitReset();
            m_bIs2dLayer = true; // this is crucial, as it causes this layer to be rendered after all the 3d info.
            m_fPercentTop    = 0;
            m_fPercentLeft   = 0;
            m_fPercentBottom = 0.5;
            m_fPercentRight  = 0.6;
		}

        ~GLImage()
        {
            if( m_pImageData ){
                free( m_pImageData );
            }
        }

        void InitReset();

		// Called by FLTK
		void draw();

		// Reposition image within the window.
		void SetPos( int nTop, int nLeft, int nBottom, int nRight );

		// Set border size.
		void SetBorder( unsigned int nSize );

        // Return image location within window
        void GetBoundingBox(int &nTop, int& nLeft, int& nBottom, int& nRight);
    
		// Return X displacement within window
		int GetX();

		// Return Y displacement within window
		int GetY();
    
        // Return Image Width within window
        int GetWidth();
    
        // Return Image Width within window
        int GetHeight();
    
        // Return Image Width
        int GetImageWidth();

        // Return Image Height
        int GetImageHeight();
    
    
        unsigned int MemSize ();

        void SetImage( unsigned char* pImageData, int w, int h, int nFormat, int nType );

        void SetSizeAsPercentageOfWindow( float fTop, float fLeft, float fBottom, float fRight );
 
	private:
		/// Allocate and bind a texture.
		void InitTexture();
    
	private:
        bool           	m_bTextureInitialized;
        bool            m_bTextureDataChanged;
        unsigned char*  m_pImageData;
        int             m_nType;    // gl
        int             m_nFormat; // gl
        int             m_nImageWidth;
        int             m_nImageHeight;
        unsigned int   	m_nTex;        	//< Texture ID associated with OpenGL texture.
        unsigned int	m_nBorder;		//< Border size.
        float           m_fPercentTop;
        float           m_fPercentLeft;
        float           m_fPercentBottom;
        float           m_fPercentRight;
};


inline void GLImage::InitTexture()
{
    if( !valid()) {
        return;
    }
    if( m_bTextureInitialized ) {
        if( m_bTextureDataChanged == true ) {
            BindRectTextureID( m_nTex, m_nImageWidth, m_nImageHeight, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_pImageData );
        }
        return;
    }
    
    if( !m_pImageData ) {
	    return;
    }

    m_bTextureInitialized = true;
    CheckForGLErrors();

    // TODO this is only accepting grayscale.. perhaps fix later?
    m_nTex = GenerateAndBindRectTextureID( m_nImageWidth, m_nImageHeight, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_pImageData );

 	CheckForGLErrors();
}


inline void GLImage::InitReset()
{
    if( m_pImageData ){
        free( m_pImageData );
    }
    // delete any saved state
    m_nTex = 0; // TODO! need to free texture memory somehow...
    m_bTextureInitialized = false;
    m_pImageData = NULL;
    m_nBorder = 0;

}


inline void GLImage::draw()
{
    int nTop = m_fPercentTop * WindowHeight();
    int nLeft = m_fPercentLeft * WindowWidth();
    int nBottom = m_fPercentBottom * WindowHeight();
    int nRight = m_fPercentRight * WindowWidth();

    glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_CURRENT_BIT | GL_LINE_BIT);

    InitTexture(); // initialize texture if necessary.
    CheckForGLErrors();

 //  glPushAttrib( GL_TEXTURE_BIT | GL_CURRENT_BIT | GL_LINE_BIT ); // this solves a glconsole bug.

    PushOrtho( WindowWidth(), WindowHeight() );

    glDisable(GL_BLEND);
    glDisable( GL_LIGHTING );
    glDisable( GL_DEPTH_TEST );

    glEnable( GL_TEXTURE_RECTANGLE_ARB );
//    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
    glBindTexture( GL_TEXTURE_RECTANGLE_ARB, m_nTex );
//    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

//    OrthoQuad( m_nImageWidth, m_nImageHeight, nTop, nLeft, nBottom, nRight );

    // actually, flip drawing dir since opengl has origin in bottom left.
//    glNormal3f( -1,0,0 );
    glColor4f( 1,1,1,1);
    glBegin( GL_QUADS );
    glTexCoord2f(           0.0,           0.0  ); glVertex3f( nLeft,  nTop, 1 );
    glTexCoord2f( m_nImageWidth,           0.0  ); glVertex3f( nRight, nTop, 1 );
    glTexCoord2f( m_nImageWidth, m_nImageHeight ); glVertex3f( nRight, nBottom, 1 );
    glTexCoord2f(           0.0, m_nImageHeight ); glVertex3f( nLeft,  nBottom, 1 );
    glEnd();

    glBindTexture( GL_TEXTURE_RECTANGLE_ARB, 0 );
    glDisable( GL_TEXTURE_RECTANGLE_ARB );

    // draw border
    m_nBorder = 0;
    if( m_nBorder != 0 ) {
        glColor3f( 0.8, 0.8, 0.8 );
        glLineWidth( m_nBorder );
        glBegin(GL_LINE_LOOP);
        glVertex2f( nLeft - 1,  nTop - 1 );
        glVertex2f( nLeft - 1,  nBottom + 1 );
        glVertex2f( nRight + 1, nBottom + 1  );
        glVertex2f( nRight + 1, nTop - 1 );
        glEnd();
    }
    PopOrtho();

    glPopAttrib();

}


inline void GLImage::SetPos( int nTop, int nLeft, int nBottom, int nRight )
{
    if( !m_pImageData ) {
        return;
    }

    m_fPercentTop = float(nTop)/WindowHeight(); 
    m_fPercentBottom = float(nBottom)/WindowHeight(); 
    m_fPercentLeft = float(nLeft)/WindowWidth(); 
    m_fPercentRight = float(nRight)/WindowWidth(); 
}


inline void GLImage::SetBorder( unsigned int nSize ) 
{
	m_nBorder = nSize;
}

inline void GLImage::GetBoundingBox(int &nTop, int& nLeft, int& nBottom, int& nRight)
{
    nTop = m_fPercentTop * WindowHeight();
    nLeft = m_fPercentLeft * WindowWidth();
    nBottom = m_fPercentBottom * WindowHeight();
    nRight = m_fPercentRight * WindowWidth(); 
}

inline int GLImage::GetX() 
{
	return m_fPercentLeft*WindowWidth();
}


inline int GLImage::GetY()
{
	return m_fPercentTop*WindowHeight();
}

inline int GLImage::GetWidth()
{
    return (m_fPercentRight - m_fPercentLeft) * WindowWidth();
}

inline int GLImage::GetHeight()
{
    return (m_fPercentBottom - m_fPercentTop) * WindowHeight();
}

inline int GLImage::GetImageWidth()
{
    return m_nImageWidth;
}

inline int GLImage::GetImageHeight()
{
    return m_nImageHeight;
}


inline unsigned int GLImage::MemSize ()
{
    if( m_pImageData ){
        return m_nImageWidth*m_nImageHeight*GLBytesPerPixel( m_nFormat, m_nType );
    }
    return 0;
}

inline void GLImage::SetImage( unsigned char* pImageData, int w, int h, int nFormat, int nType )
{
    if( !pImageData ) {
        return;
    }

    int nBpp = GLBytesPerPixel( nFormat, nType );
    unsigned int nMemSize = nBpp*w*h;

    if( nMemSize != MemSize() ){
        if( m_pImageData ) {
            free( m_pImageData );
        }
        m_pImageData = (unsigned char*)malloc( nMemSize );
        m_bTextureInitialized = false;
        m_bTextureDataChanged = false;
    }else {
        m_bTextureInitialized = true;
        m_bTextureDataChanged = true;
    }

    memcpy( m_pImageData, pImageData, nMemSize );

    m_nImageWidth = w;
    m_nImageHeight = h;
    m_nFormat = nFormat;
    m_nType = nType;

    
}

inline void GLImage::SetSizeAsPercentageOfWindow( 
        float fTop, float fLeft, float fBottom, float fRight 
        )
{
    m_fPercentTop    = fTop;
    m_fPercentLeft   = fLeft;
    m_fPercentBottom = fBottom;
    m_fPercentRight  = fRight;
}

} // SceneGraph

#endif
