#ifndef _GLCUBE_H_
#define _GLCUBE_H_

#include <SceneGraph/GLObject.h>
#include <math.h>

namespace SceneGraph
{

///////////////////////////////////////////////////////////////////////////////
// GLCube
///////////////////////////////////////////////////////////////////////////////

class GLCube : public GLObject
{
public:
    GLCube();

    GLCube(GLuint texId);

    void ClearTexture();

    void SetCheckerboard();

    void SetTexture(GLuint texId);

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

inline void GLCube::ClearTexture()
{
    if(m_nTexID > 0) {
        if(m_bOwnsTexture) {
            glDeleteTextures(1,&m_nTexID);
        }
        m_nTexID = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////

inline void GLCube::SetCheckerboard()
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
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D( GL_TEXTURE_2D, 0,
            GL_RGB, TEX_W, TEX_H, 0, GL_RGB, GL_UNSIGNED_BYTE, &img[0][0][0] );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}

///////////////////////////////////////////////////////////////////////////////

inline void GLCube::SetTexture(GLuint texId)
{
    ClearTexture();

    m_bOwnsTexture = false;
    m_nTexID = texId;
}

///////////////////////////////////////////////////////////////////////////////

inline GLCube::GLCube()
    : m_bOwnsTexture(false), m_nTexID(0)
{
    m_aabb.Min() = Eigen::Vector3d(-0.5,-0.5,-0.5);
    m_aabb.Max() = Eigen::Vector3d(+0.5,+0.5,+0.5);
    SetCheckerboard();
}

///////////////////////////////////////////////////////////////////////////////

inline GLCube::GLCube(GLuint texId)
    : m_bOwnsTexture(false), m_nTexID(0)
{
    m_aabb.Min() = Eigen::Vector3d(-0.5,-0.5,-0.5);
    m_aabb.Max() = Eigen::Vector3d(+0.5,+0.5,+0.5);
    SetTexture(texId);
}

///////////////////////////////////////////////////////////////////////////////

inline void GLCube::DrawCanonicalObject()
{
#ifndef ANDROID
    if(m_nTexID) {
        glEnable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, m_nTexID );
    }

    glColor3f(1,1,1);

    const Eigen::Vector3d bmin = m_aabb.Min();
    const Eigen::Vector3d bmax = m_aabb.Max();

    // Draw cube with texture assigned to each face
    glBegin(GL_QUADS);
    // Front Face
    glNormal3f(0,0,1);
    glTexCoord2f(0.0, 1.0); glVertex3f(bmin(0), bmax(1), bmax(2));
    glTexCoord2f(1.0, 1.0); glVertex3f(bmax(0), bmax(1), bmax(2));
    glTexCoord2f(1.0, 0.0); glVertex3f(bmax(0), bmin(1), bmax(2));
    glTexCoord2f(0.0, 0.0); glVertex3f(bmin(0), bmin(1), bmax(2));
    // Back Face
    glNormal3f(0,0,-1);
    glTexCoord2f(0.0, 1.0); glVertex3f(bmax(0), bmax(1), bmin(2));
    glTexCoord2f(1.0, 1.0); glVertex3f(bmin(0), bmax(1), bmin(2));
    glTexCoord2f(1.0, 0.0); glVertex3f(bmin(0), bmin(1), bmin(2));
    glTexCoord2f(0.0, 0.0); glVertex3f(bmax(0), bmin(1), bmin(2));
    // Top Face
    glNormal3f(0,1,0);
    glTexCoord2f(0.0, 1.0); glVertex3f(bmin(0), bmax(1), bmin(2));
    glTexCoord2f(1.0, 1.0); glVertex3f(bmax(0), bmax(1), bmin(2));
    glTexCoord2f(1.0, 0.0); glVertex3f(bmax(0), bmax(1), bmax(2));
    glTexCoord2f(0.0, 0.0); glVertex3f(bmin(0), bmax(1), bmax(2));
    // Bottom Face
    glNormal3f(0,-1,0);
    glTexCoord2f(0.0, 1.0); glVertex3f(bmax(0), bmin(1), bmin(2));
    glTexCoord2f(1.0, 1.0); glVertex3f(bmin(0), bmin(1), bmin(2));
    glTexCoord2f(1.0, 0.0); glVertex3f(bmin(0), bmin(1), bmax(2));
    glTexCoord2f(0.0, 0.0); glVertex3f(bmax(0), bmin(1), bmax(2));
    // Right face
    glNormal3f(1,0,0);
    glTexCoord2f(0.0, 1.0); glVertex3f(bmax(0), bmax(1), bmax(2));
    glTexCoord2f(1.0, 1.0); glVertex3f(bmax(0), bmax(1), bmin(2));
    glTexCoord2f(1.0, 0.0); glVertex3f(bmax(0), bmin(1), bmin(2));
    glTexCoord2f(0.0, 0.0); glVertex3f(bmax(0), bmin(1), bmax(2));
    // Left Face
    glNormal3f(-1,0,0);
    glTexCoord2f(0.0, 1.0); glVertex3f(bmin(0), bmax(1), bmin(2));
    glTexCoord2f(1.0, 1.0); glVertex3f(bmin(0), bmax(1), bmax(2));
    glTexCoord2f(1.0, 0.0); glVertex3f(bmin(0), bmin(1), bmax(2));
    glTexCoord2f(0.0, 0.0); glVertex3f(bmin(0), bmin(1), bmin(2));
    glEnd();

    if(m_nTexID) {
        glDisable( GL_TEXTURE_2D );
    }
#endif  // ANDROID
}

}

#endif //_GLCube_H_
