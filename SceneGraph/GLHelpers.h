/*
 *  \file GLHelpers.h
 *
 * $Id$
 */ 
 
#ifndef _GL_HELPERS_H_
#define _GL_HELPERS_H_

#include <GL/glew.h>
#include <GL/glext.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#undef Success
//#include <Eigen/Matrix>
#include <Eigen/Core>
//#include <Eigen/LU>

namespace SceneGraph
{

#define CheckForGLErrors() _CheckForGLErrors( __FILE__, __LINE__ );

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
unsigned int GLBytesPerPixel( unsigned int nFormat, unsigned int nType );

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

////////////////////////////////////////////////////////////////////////////////////////////////////
void DrawCamera(
        int nTexWidth,
        int nTexHeight,
        int nTexId,
        Eigen::Matrix4d dModelViewMatrix,
        Eigen::Matrix4d dProjectionMatrix
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

} // SceneGraph


#endif
