#ifndef _GLCYLINDER_H_
#define _GLCYLINDER_H_

#include <SceneGraph/GLObject.h>

namespace SceneGraph
{

class GLCylinder : public GLObject
{
    public:
        GLCylinder( GLuint texId = 0 );
        ~GLCylinder();

        void Init( 
                double dBbase,
                double dTop,
                double dHeight,
                int    nSlices,
                int    nStacks 
                );

        void ClearTexture();

        void SetCheckerboard();

        void SetTexture(GLuint texId);

        void DrawCanonicalObject();

    protected:
        GLUquadric*   m_pQuadric;
        GLUquadric*   m_pTopDisk;
        GLUquadric*   m_pBottomDisk;

        double        m_dBaseRadius;
        double        m_dTopRadius;
        double        m_dHeight;
        double        m_nSlices;    
        double        m_nStacks;
        bool          m_bDrawCaps;

        bool   m_bOwnsTexture;
        GLuint m_nTexID;

        const static int TEX_W = 64;
        const static int TEX_H = 64;
};

//inline GLCylinder::GLCylinder()
inline GLCylinder::GLCylinder( GLuint texId )
{
    m_bOwnsTexture = false;
    m_nTexID = texId;
    m_pQuadric = NULL;
    m_bDrawCaps = true;
    SetCheckerboard();
}

inline GLCylinder::~GLCylinder()
{
    if( m_pQuadric ){
        gluDeleteQuadric( m_pQuadric );
    }
    if( m_pTopDisk ){
        gluDeleteQuadric( m_pTopDisk );
    }
    if( m_pBottomDisk ){ 
        gluDeleteQuadric( m_pBottomDisk );
    }
}

inline void GLCylinder::Init( 
        double dBaseRadius,
        double dTopRadius,
        double dHeight,
        int    nSlices,
        int    nStacks 
        )
{
    if( m_pQuadric ){
        gluDeleteQuadric( m_pQuadric );
    }
    if( m_pTopDisk ){
        gluDeleteQuadric( m_pTopDisk );
    }
    if( m_pBottomDisk ){ 
        gluDeleteQuadric( m_pBottomDisk );
    }

    if( m_bDrawCaps ){
        m_pTopDisk = gluNewQuadric();
        gluQuadricNormals( m_pTopDisk, GLU_SMOOTH );
        gluQuadricDrawStyle( m_pTopDisk, GLU_FILL );
        gluQuadricTexture( m_pTopDisk, GL_TRUE );

        m_pBottomDisk = gluNewQuadric();
        gluQuadricNormals( m_pBottomDisk, GLU_SMOOTH );
        gluQuadricDrawStyle( m_pBottomDisk, GLU_FILL );
        gluQuadricTexture( m_pBottomDisk, GL_TRUE );

    }


    m_pQuadric = gluNewQuadric();
    gluQuadricNormals( m_pQuadric, GLU_SMOOTH );
    gluQuadricDrawStyle( m_pQuadric, GLU_FILL );
    gluQuadricTexture( m_pQuadric, GL_TRUE );

    m_dBaseRadius = dBaseRadius;
    m_dTopRadius  = dTopRadius;
    m_dHeight     = dHeight;
    m_nSlices     = nSlices;
    m_nStacks     = nStacks;
}

inline void GLCylinder::ClearTexture()
{
    if(m_nTexID > 0) {
        if(m_bOwnsTexture) {
            glDeleteTextures(1,&m_nTexID);
        }
        m_nTexID = 0;
    }

}

inline void GLCylinder::SetCheckerboard()
{
    ClearTexture();

    // Texture Map Init
    GLubyte img[TEX_W][TEX_H][3]; // after glTexImage2D(), array is no longer needed
    for (int x=0; x<TEX_W; x++) {
        for (int y=0; y<TEX_H; y++) {
            GLubyte c = ((x&16)^(y&16)) ? 255 : 0; // checkerboard
            img[x][y][0] = c;
            img[x][y][1] = c;
            img[x][y][2] = c;
        }
    }

    // Generate and bind the texture
    m_bOwnsTexture = true;
    glGenTextures( 1, &m_nTexID );
    glBindTexture( GL_TEXTURE_2D, m_nTexID );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D( GL_TEXTURE_2D, 0,
            GL_RGB, TEX_W, TEX_H, 0, GL_RGB, GL_UNSIGNED_BYTE, &img[0][0][0] );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}

inline void GLCylinder::SetTexture( GLuint texId )
{
    ClearTexture();
    m_bOwnsTexture = false;
    m_nTexID = texId;
}

inline void GLCylinder::DrawCanonicalObject()
{
    if(m_nTexID) {
        glEnable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, m_nTexID );
    }

    if( m_pQuadric ){
        gluCylinder( m_pQuadric, m_dBaseRadius, m_dTopRadius, m_dHeight, m_nSlices, m_nStacks );

        if( m_bDrawCaps ){
            gluDisk( m_pBottomDisk, 0, m_dBaseRadius, m_nSlices, 2 );
            glTranslatef( 0, 0, m_dHeight );
            gluDisk( m_pTopDisk, 0, m_dTopRadius, m_nSlices, 2 );
        }
    }
}


}

#endif
