/*
 *  \file GLHelpers.h
 *
 * $Id$
 */

#ifndef _GL_HELPERS_H_
#define _GL_HELPERS_H_

#include <SceneGraph/config.h>
#include <SceneGraph/GLinclude.h>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#undef Success
#include <Eigen/Core>

namespace SceneGraph
{

#define CheckForGLErrors() SceneGraph::_CheckForGLErrors( __FILE__, __LINE__ );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///  If a GL error has occured, this function outputs "msg" and the
//   programme exits. To avoid exiting @see WarnForGLErrors.
void _CheckForGLErrors( const char *sFile = NULL, const int nLine = -1 );

//////////////////////////////////////////////////////////////////////////////
// Extract current camera pose from opengl in the Robotics Coordinate frame convention
Eigen::Matrix4d GLGetCameraPose();

///////////////////////////////////////////////////////////////////////////////
/// Convert opengl projection matrix into computer vision K matrix
Eigen::Matrix3d GLGetProjectionMatrix();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// If a GL error has occured, this function outputs "msg" and  automatically sets
//  the gl error state back to normal.
void WarnForGLErrors( const char * msg = NULL );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Given a particular OpenGL Format, return the appropriate number of image channels.
unsigned int NumChannels( unsigned int nFormat );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Given an OpenGL Type, return the associated number of bits used.
unsigned int BitsPerChannel( unsigned int nType );

////////////////////////////////////////////////////////////////////////////////////////////////////
// Change to orthographic projection (for image drawing, etc)
void PushOrtho( const unsigned int nWidth, const unsigned int nHeight );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Set projection matrix
void PopOrtho();

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Set projection matrix
void OrthoQuad(
        const int nTexWidth,    //< Input:
        const int nTexHeight,   //< Input:
        const int nTop,      //< Input:
        const int nLeft,     //< Input:
        const int nBottom,   //< Input:
        const int nRight     //< Input:
        );

////////////////////////////////////////////////////////////////////////////////////////////////////
void DrawBorderAsWindowPercentage(
        const float fTop,
        const float fLeft,
        const float fBottom,
        const float fRight
        );

////////////////////////////////////////////////////////////////////////////////////////////////////
void DrawTextureAsWindowPercentage(
        const unsigned int nTexId,      //< Input:
        const unsigned int nTexWidth, //< Input:
        const unsigned int nTexHeight,//< Input:
        const float fTop,
        const float fLeft,
        const float fBottom,
        const float fRight
    );


////////////////////////////////////////////////////////////////////////////////////////////////////
void DrawTexture(
        const unsigned int nTexId,      //< Input:
        const unsigned int nTexWidth, //< Input:
        const unsigned int nTexHeight,//< Input:
        const unsigned int nTop,        //< Input:
        const unsigned int nLeft,       //< Input:
        const unsigned int nBottom,     //< Input:
        const unsigned int nRight       //< Input:
        );

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline unsigned int GLBytesPerPixel( unsigned int nFormat, unsigned int nType )
{
    unsigned nVal = 0;
    switch ( nType ) {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
            nVal = 1;
            break;
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
            nVal = 2;
            break;
#ifndef HAVE_GLES
        case GL_2_BYTES:
            nVal = 2;
            break;
        case GL_3_BYTES:
            nVal = 3;
            break;
        case GL_INT:
        case GL_UNSIGNED_INT:
        case GL_4_BYTES:
        case GL_FLOAT:
            nVal = 4;
            break;
        case GL_DOUBLE:
            nVal = 8;
#endif
    }
    
    switch ( nFormat ) {
#ifndef HAVE_GLES
        case GL_RED:
        case GL_R8:
        case GL_R8UI:
#endif
        case GL_LUMINANCE:
            break;
        case GL_RGB: nVal*=3; break;
        case GL_RGBA: nVal*=4; break;
        default:
            fprintf ( stderr, "GLBytesPerPixel() -- unsupported format\n" );
            return 0;
    }
    return nVal;    
}

//////////////////////////////////////////////////////////////////////////////
// read from opengl buffer into our own vector
void ReadPixels(
        std::vector<unsigned char>& vPixels,
        int nWidth,
        int nHeight,
        bool bFlip = true
        );

//////////////////////////////////////////////////////////////////////////////
// read from opengl buffer into our own vector
void ReadDepthPixels(
        std::vector<float>& vPixels,
        int nWidth,
        int nHeight,
        bool bFlip = true
        );

//////////////////////////////////////////////////////////////////////////////
void CheckFBOStatus();

////////////////////////////////////////////////////////////////////////////////////////////////////
/// set perspective view.  same as gluperspective().
void Perspective( double fovy, double aspect, double zNear, double zFar);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Reshape viewport whenever window changes size.
void ReshapeViewport( int w, int h );


// TODO: Should these really be here?
inline Eigen::Matrix4d GLCart2T(
                              double x,
                              double y,
                              double z,
                              double r,
                              double p,
                              double q
                              )
{
    Eigen::Matrix4d T;
    // psi = roll, th = pitch, phi = yaw
    double cq, cp, cr, sq, sp, sr;
    cr = cos( r );
    cp = cos( p );
    cq = cos( q );

    sr = sin( r );
    sp = sin( p );
    sq = sin( q );

    T(0,0) = cp*cq;
    T(0,1) = -cr*sq+sr*sp*cq;
    T(0,2) = sr*sq+cr*sp*cq;

    T(1,0) = cp*sq;
    T(1,1) = cr*cq+sr*sp*sq;
    T(1,2) = -sr*cq+cr*sp*sq;

    T(2,0) = -sp;
    T(2,1) = sr*cp;
    T(2,2) = cr*cp;

    T(0,3) = x;
    T(1,3) = y;
    T(2,3) = z;
    T.row(3) = Eigen::Vector4d( 0.0, 0.0, 0.0, 1.0 );
    return T;
}

inline Eigen::Matrix4d GLCart2T( const Eigen::Matrix<double,6,1>& x)
{
    return GLCart2T(x(0),x(1),x(2),x(3),x(4),x(5));
}

inline Eigen::Vector3d GLR2Cart(
        const Eigen::Matrix3d& R
        )
{
    Eigen::Vector3d rpq;
    // roll
    rpq[0] = atan2( R(2,1), R(2,2) );

    // pitch
    double det = -R(2,0) * R(2,0) + 1.0;
    if (det <= 0) {
        if (R(2,0) > 0){
            rpq[1] = -M_PI / 2.0;
        }
        else{
            rpq[1] = M_PI / 2.0;
        }
    }
    else{
        rpq[1] = -asin(R(2,0));
    }

    // yaw
    rpq[2] = atan2(R(1,0), R(0,0));

    return rpq;
}

inline Eigen::Matrix<double,6,1> GLT2Cart(
        const Eigen::Matrix4d& T
        )
{
    Eigen::Matrix<double,6,1> Cart;
    Eigen::Vector3d rpq = GLR2Cart( T.block<3,3>(0,0) );
    Cart[0] = T(0,3);
    Cart[1] = T(1,3);
    Cart[2] = T(2,3);
    Cart[3] = rpq[0];
    Cart[4] = rpq[1];
    Cart[5] = rpq[2];

    return Cart;
}


// TODO: Should these really be here?
inline Eigen::Matrix3d GLCart2R(
                     const double& r,
                     const double& p,
                     const double& q
                     )
{
    Eigen::Matrix3d R;
    // psi = roll, th = pitch, phi = yaw
    double cq, cp, cr, sq, sp, sr;
    cr = cos( r );
    cp = cos( p );
    cq = cos( q );

    sr = sin( r );
    sp = sin( p );
    sq = sin( q );

    R(0,0) = cp*cq;
    R(0,1) = -cr*sq+sr*sp*cq;
    R(0,2) = sr*sq+cr*sp*cq;

    R(1,0) = cp*sq;
    R(1,1) = cr*cq+sr*sp*sq;
    R(1,2) = -sr*cq+cr*sp*sq;

    R(2,0) = -sp;
    R(2,1) = sr*cp;
    R(2,2) = cr*cp;
    return R;
}

inline Eigen::Matrix3d Rotation_a2b(const Eigen::Vector3d& a, const Eigen::Vector3d& b)
{
    Eigen::Vector3d n = a.cross(b);

    if(n.squaredNorm() == 0) {
        // TODO: Should this be identity?
        return Eigen::Matrix3d::Identity();
    }

    n.normalize();
    Eigen::Matrix3d R1;
    R1.col(0) = a.normalized();
    R1.col(1) = n;
    R1.col(2) = n.cross(R1.col(0));

    Eigen::Matrix3d M;
    M.col(0) = b.normalized();
    M.col(1) = n;
    M.col(2) = n.cross(M.col(0));
    M = M * R1.transpose();
    return M;
}

#ifndef HAVE_GLES

////////////////////////////////////////////////////////////////////////////////////////////////////
void DrawCamera(
        int nTexWidth,
        int nTexHeight,
        int nTexId,
        const Eigen::Matrix4d& dModelViewMatrix,
        const Eigen::Matrix4d& dProjectionMatrix
        );

/// 1) Generate a texutre ID
//  2) Bind texture to memory
//  3) Load texture into memory
inline void BindRectTextureID(
        const unsigned int texId,
        const unsigned int nWidth,
        const unsigned int nHeight,
        const unsigned int nFormat,
        const unsigned int nType,
        const unsigned char* pData
        )
{
    glBindTexture( GL_TEXTURE_RECTANGLE_ARB, texId );
    glTexImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, nWidth, nHeight, 0, nFormat, nType, pData );
}

/// 1) Generate a texutre ID
//  2) Bind texture to memory
//  3) Load texture into memory
inline unsigned int GenerateAndBindRectTextureID(
        const unsigned int nWidth,
        const unsigned int nHeight,
        const unsigned int nFormat,
        const unsigned int nType,
        const unsigned char* pData
        )
{
    GLuint texId;
    glGenTextures( 1, &texId );
    glBindTexture( GL_TEXTURE_RECTANGLE_ARB, texId );
    glTexImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, nWidth, nHeight, 0, nFormat, nType, pData );
    return texId;
}



/// 1) Generate a texutre ID
//  2) Bind texture to memory
//  3) Load texture into memory
inline unsigned int GenerateAndBindTextureID(
        const unsigned int nWidth,
        const unsigned int nHeight,
        const unsigned int nFormat,
        const unsigned int nType,
        const unsigned char* pData
        )
{
    GLuint texId;

    /// Ask for an ID
    glGenTextures( 1, &texId );

    /// Associate that ID with the next thing we upload.
    glBindTexture( GL_TEXTURE_2D, texId );

    /// Texture Mapping Mode:
    // GL_DECAL: use actual texture colors
    // GL_MODULATE: texture colors affected by poly's color (this is the default).
//    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );

    // GL_UNPACK_ALIGNMENT Specifies the alignment requirements for the start of each
    //  pixel row in memory.  1 specifies byte-alignment.
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1);

    // Actually copy the texture into opengl
    glTexImage2D(
            GL_TEXTURE_2D,  // texture target
            0,              // mipmap level
            GL_RGB,         // internal format
            nWidth,         // texture widht
            nHeight,        // texture height
            0,              // width of the border (0 or 1)
            nFormat,        // pixel data format
            nType,          // pixel data type
            pData           // pixel data
            );

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    /*
    //
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    //
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );

    //
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

    //
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    //
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    //    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
     */

    return texId;
}

// TODO: eliminate this in favour of above methods? Or vice versa
inline GLuint LoadGLTexture(GLint width, GLint height, void* data, GLint internal_format = GL_RGB8, GLenum data_layout = GL_RGB, GLenum data_type = GL_UNSIGNED_BYTE )
{
    GLuint glTexId = 0;
    glGenTextures(1,&glTexId);

    glBindTexture(GL_TEXTURE_2D, glTexId);
    // load Mipmaps instead of single texture
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, data_layout, data_type, data);
//            glGenerateMipmap( GL_TEXTURE_2D );
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, data_layout, data_type, data );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glBindTexture(GL_TEXTURE_2D, 0);

    return glTexId;
}
#endif // HAVE_GLES


} // SceneGraph


#endif
