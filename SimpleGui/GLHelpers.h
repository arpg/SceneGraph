/*
 *  \file GLHelpers.h
 *
 * $Id$
 */ 
 
#ifndef _GL_HELPERS_H_
#define _GL_HELPERS_H_


#ifdef HAVE_APPLE_OPENGL_FRAMEWORK
#    include <FL/gl.h>
#    include <FL/glu.h>
#    include <FL/glut.H>
#else
#    include <GL/glew.h>
#    include <GL/glext.h>
#    include <FL/gl.h>
#    include <FL/glu.h>
#    include <FL/glut.H>
#endif

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

#endif
