#ifndef _GLBOX_H_
#define _GLBOX_H_

#include <SceneGraph/GLObject.h>
#include <math.h>

namespace SceneGraph
{

    ///////////////////////////////////////////////////////////////////////////////
    // GLBox
    ///////////////////////////////////////////////////////////////////////////////
    class GLBox : public GLObject
    {
        public:
            GLBox();

            GLBox(GLuint texId);

            void ClearTexture();

            void SetCheckerboard();

            void SetTexture(GLuint texId);
    
            void SetExtent( double dHalfDepth, double dHalfWidth, double dHalfHeight );

            void DrawCanonicalObject();

        protected:
            const static int TEX_W = 64;
            const static int TEX_H = 64;
            bool   m_bOwnsTexture;
            GLuint m_nTexID;
    };

    ///////////////////////////////////////////////////////////////////////////////
    // Implementation
    ///////////////////////////////////////////////////////////////////////////////
    inline void GLBox::ClearTexture()
    {
        if(m_nTexID > 0) {
            if(m_bOwnsTexture) {
                glDeleteTextures(1,&m_nTexID);
            }
            m_nTexID = 0;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    inline void GLBox::SetCheckerboard()
    {
        ClearTexture();

        // Texture Map Init
        GLubyte img[TEX_W][TEX_H][3]; // after glTexImage2D(), array is no longer needed
        for (int x=0; x<TEX_W; x++) {
            for (int y=0; y<TEX_H; y++) {
                GLubyte c = ((x&16)^(y&16)) ? 255 : 100; // checkerboard
                img[x][y][0] = 0.8*c;
                img[x][y][1] = 0.6*c;
                img[x][y][2] = 1.0*c;
            }
        }

        // Generate and bind the texture
        m_bOwnsTexture = true;
        glGenTextures( 1, &m_nTexID );
        glBindTexture( GL_TEXTURE_2D, m_nTexID );
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glPixelStorei( GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D( GL_TEXTURE_2D, 0,
                GL_RGB, TEX_W, TEX_H, 0, GL_RGB, GL_UNSIGNED_BYTE, &img[0][0][0] );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    ///////////////////////////////////////////////////////////////////////////////
    inline void GLBox::SetTexture(GLuint texId)
    {
        ClearTexture();

        m_bOwnsTexture = false;
        m_nTexID = texId;
    }

    ///////////////////////////////////////////////////////////////////////////////
    inline void GLBox::SetExtent( double dHalfDepth, double dHalfWidth, double dHalfHeight )
    {
        m_aabb.Min() = Eigen::Vector3d( -dHalfDepth/2.0, -dHalfWidth/2.0, -dHalfHeight/2.0 );
        m_aabb.Max() = Eigen::Vector3d(  dHalfDepth/2.0,  dHalfWidth/2.0,  dHalfHeight/2.0 );
    }

    ///////////////////////////////////////////////////////////////////////////////
    inline GLBox::GLBox()
        : m_bOwnsTexture(false), m_nTexID(0)
    {
        SetExtent( 1, 1, 1 );
        SetCheckerboard();
    }

    ///////////////////////////////////////////////////////////////////////////////
    inline GLBox::GLBox(GLuint texId)
        : m_bOwnsTexture(false), m_nTexID(0)
    {
        SetExtent( 1, 1, 1 );
        SetTexture(texId);
    }

    ///////////////////////////////////////////////////////////////////////////////
    inline void GLBox::DrawCanonicalObject()
    {
        if(m_nTexID) {
            glEnable( GL_TEXTURE_2D );
            glBindTexture( GL_TEXTURE_2D, m_nTexID );
        }

        glColor3f(1,1,1);

        const Eigen::Vector3d bmin = m_aabb.Min();
        const Eigen::Vector3d bmax = m_aabb.Max();

        // Draw cube with texture assigned to each face
        glBegin(GL_QUADS);
        // +z Face
        glNormal3f(0,0,1);
        glTexCoord2f(     0.0, bmax(1) ); glVertex3f( bmin(0), bmax(1), bmax(2) );
        glTexCoord2f( bmax(0), bmax(1) ); glVertex3f( bmax(0), bmax(1), bmax(2) );
        glTexCoord2f( bmax(0),     0.0 ); glVertex3f( bmax(0), bmin(1), bmax(2) );
        glTexCoord2f(     0.0,     0.0 ); glVertex3f( bmin(0), bmin(1), bmax(2) );
        
        // -z Face
        glNormal3f(0, 0,-1);
        glTexCoord2f(     0.0, bmax(1) ); glVertex3f( bmax(0), bmax(1), bmin(2) );
        glTexCoord2f( bmax(0), bmax(1) ); glVertex3f( bmin(0), bmax(1), bmin(2) );
        glTexCoord2f( bmax(0),     0.0 ); glVertex3f( bmin(0), bmin(1), bmin(2) );
        glTexCoord2f(     0.0,     0.0 ); glVertex3f( bmax(0), bmin(1), bmin(2) );
 
        // +y Face
        glNormal3f(0, 1,0);
        glTexCoord2f(     0.0, bmax(2) ); glVertex3f( bmin(0), bmax(1), bmin(2) );
        glTexCoord2f( bmax(0), bmax(2) ); glVertex3f( bmax(0), bmax(1), bmin(2) );
        glTexCoord2f( bmax(0),     0.0 ); glVertex3f( bmax(0), bmax(1), bmax(2) );
        glTexCoord2f(     0.0,     0.0 ); glVertex3f( bmin(0), bmax(1), bmax(2) );

        // -y Face
        glNormal3f(0, -1,0);
        glTexCoord2f(     0.0, bmax(2) ); glVertex3f( bmax(0), bmin(1), bmin(2) );
        glTexCoord2f( bmax(0), bmax(2) ); glVertex3f( bmin(0), bmin(1), bmin(2) );
        glTexCoord2f( bmax(0),     0.0 ); glVertex3f( bmin(0), bmin(1), bmax(2) );
        glTexCoord2f(     0.0,     0.0 ); glVertex3f( bmax(0), bmin(1), bmax(2) );
        
        // +x face
        glNormal3f(1, 0,0);
        glTexCoord2f(     0.0, bmax(1) ); glVertex3f( bmax(0), bmax(1), bmax(2) );
        glTexCoord2f( bmax(2), bmax(1) ); glVertex3f( bmax(0), bmax(1), bmin(2) );
        glTexCoord2f( bmax(2),     0.0 ); glVertex3f( bmax(0), bmin(1), bmin(2) );
        glTexCoord2f(     0.0,     0.0 ); glVertex3f( bmax(0), bmin(1), bmax(2) );

        // -x Face
        glNormal3f(-1 ,0,0);
        glTexCoord2f(     0.0, bmax(1) ); glVertex3f( bmin(0), bmax(1), bmin(2) );
        glTexCoord2f( bmax(2), bmax(1) ); glVertex3f( bmin(0), bmax(1), bmax(2) );
        glTexCoord2f( bmax(2),     0.0 ); glVertex3f( bmin(0), bmin(1), bmax(2) );
        glTexCoord2f(     0.0,     0.0 ); glVertex3f( bmin(0), bmin(1), bmin(2) );        
        
        glEnd();

        if(m_nTexID) {
            glDisable( GL_TEXTURE_2D );
        }
    }

}

#endif //_GLBOX_H_

