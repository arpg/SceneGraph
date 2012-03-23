/*
 *  \file GLHelpers.h
 *
 *  $Id: GLHelpers.h 177 2010-04-18 23:40:37Z effer $
 */ 

#ifndef _GLHELPERS_H_
#define _GLHELPERS_H_

#ifdef HAVE_APPLE_OPENGL_FRAMEWORK
#    include <GL/glew.h>
#    include <GL/glext.h>
#    include <OpenGL/gl.h>
#    include <OpenGL/glu.h>
#    include <GLUT/glut.h>
#else
#    include <GL/glew.h>
#    include <GL/glext.h>
#    include <GL/gl.h>
#    include <GL/glu.h>
#    include <GL/glut.h>
#endif

#include <Fl/gl.h>
#include <FL/glu.h>

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <string>
#include <Eigen/Core>
#include <Eigen/LU>

//////////////////////////////////////////////////////////////////////////////                       
// Extract current camera pose from opengl in the Robotics Coordinate frame convention               
inline Eigen::Matrix4d GLGetCameraPose()                                                             
{
    Eigen::Matrix<double,4,4,Eigen::ColMajor> M; // for opengl                                       
    glGetDoublev( GL_MODELVIEW_MATRIX, M.data() );                                                   

    Eigen::Matrix3d gl2v;                                                                            
    gl2v << 0,0,-1,  1,0,0,  0,-1,0;                                                                 

    Eigen::Vector3d xyz = -M.block<3,3>(0,0).transpose()*M.block<3,1>(0,3);                          
    //    Vector3d pqr = R2Cart( gl2v*M.block<3,3>(0,0) );                                               

    Eigen::Matrix4d T;                                                                               

    T.block<3,3>(0,0) = gl2v*M.block<3,3>(0,0);                                                      
    T.block<3,1>(0,3) = -M.block<3,3>(0,0).transpose()*M.block<3,1>(0,3);                            
    T.block<1,4>(3,0) << 0, 0, 0, 1;                                                                 

    return T;
}



///////////////////////////////////////////////////////////////////////////////                      
/// Convert opengl projection matrix into computer vision K matrix
inline Eigen::Matrix3d GLGetProjectionMatrix()
{                                                                                                    
    Eigen::Matrix<double,4,4,Eigen::ColMajor> P; // for opengl                                       
    glGetDoublev( GL_PROJECTION_MATRIX, P.data() );                                                  
    GLint vViewport[4];
    glGetIntegerv( GL_VIEWPORT, vViewport );                                                         

    //    fovy = 2*atan( h/2/f );
    //    f = tan(fovy/2)/(h/2)

    Eigen::Matrix3d K;
    K(0,0) = P(0,0); // P00 = 1/tan(fov/2) ==> fx = P00                                              
    K(0,1) = 0.0; // sx                                                                              
    K(0,2) = vViewport[2]/2.0; // cx                                                                 
    K(1,0) = 0.0;
    K(1,1) = P(1,1); // fy
    K(1,2) = vViewport[3]/2.0; // cy                                                                 
    K(2,0) = 0.0;
    K(2,1) = 0.0;
    K(2,2) = 1.0;                                                                                    

    //    cout << P(0,0) * vViewport[2]/(double)vViewport[3] << endl;                                    
    //    cout << "K:\n" << K <<  endl << endl;                                                          

    return K;
} 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///  If a GL error has occurred, this function outputs "msg" and the
//   program exits. To avoid exiting @see WarnForGLErrors.
inline void CheckForGLErrors( const char * msg = NULL )
{
    GLenum glError = glGetError();
    if( glError != GL_NO_ERROR ) {
        if( msg ) {
            fprintf( stderr, "ERROR: CheckForGLErrors() -- %s\n", msg );
        }
        fprintf( stderr, "ERROR: %s\n", (char *) gluErrorString(glError) );
        exit( -1 );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// If a GL error has occurred, this function outputs "msg" and  automatically sets
//  the gl error state back to normal.
inline void WarnForGLErrors( const char * msg = NULL )
{
    GLenum glError = (GLenum)glGetError();
    if( glError != GL_NO_ERROR ) {
        if( msg ) {
            fprintf( stderr, "WARNING: %s\n", msg );
        }
        fprintf( stderr, "ERROR: %s\n", (char *) gluErrorString(glError) );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Given a particular OpenGL Format, return the appropriate number of image channels.
inline unsigned int NumChannels( unsigned int nFormat )
{
    switch( nFormat ){
        case GL_LUMINANCE: 
            return 1;
        case GL_RGB:
            return 3;
        case GL_RGBA: 
            return 4;
        default:
            fprintf( stderr, "NumChannels() - unknown format\n" );
            return 0;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Given an OpenGL Type, return the associated number of bits used.
inline unsigned int BitsPerChannel( unsigned int nType )
{
    switch( nType ){
        case GL_UNSIGNED_BYTE:
        case GL_BYTE:
            return 8;
        case GL_2_BYTES:
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
            return 16;
        case GL_4_BYTES:
        case GL_FLOAT:
        case GL_UNSIGNED_INT:
        case GL_INT:
            return 32;
        case GL_DOUBLE:
            return 64;
        case GL_3_BYTES:
            return 24;
        default:
            fprintf( stderr, "\nBitsPerChannel() - unknown image Type");
            return 0;
    }
    return 0;
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

////////////////////////////////////////////////////////////////////////////////////////////////////
// Change to orthographic projection (for image drawing, etc)
inline void PushOrtho( const unsigned int nWidth, const unsigned int nHeight )
{
    // load ortho to the size of the window 
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, nWidth, nHeight, 0, -1, 1); // left, right, bottom, top, near, far
    glMatrixMode( GL_MODELVIEW );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Set projection matrix
inline void PopOrtho()
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Set projection matrix
inline void OrthoQuad( 
        const int nWidth,    //< Input:
        const int nHeight,   //< Input:
        const int nTop,      //< Input:
        const int nLeft,     //< Input:
        const int nBottom,   //< Input:
        const int nRight     //< Input:
        )
{
    glBegin( GL_QUADS );
    glTexCoord2f(    0.0,    0.0  ); glVertex3f( nLeft,  nTop, 1 );
    glTexCoord2f( nWidth,    0.0  ); glVertex3f( nRight, nTop, 1 );
    glTexCoord2f( nWidth, nHeight ); glVertex3f( nRight, nBottom, 1 );
    glTexCoord2f(    0.0, nHeight ); glVertex3f( nLeft,  nBottom, 1 );
    glEnd();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// set perspective view.  same as gluperspective().
inline void Perspective( double fovy, double aspect, double zNear, double zFar)
{
    double xmin, xmax, ymin, ymax;
    ymax = zNear * tan(fovy * M_PI / 360.0);
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;
    glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Reshape viewport whenever window changes size.
inline void ReshapeViewport( int w, int h )
{
    // Viewport
    glViewport(0, 0, w, h );

    // Projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat ratio = float(w) / float(h);
    Perspective(45.0, 1.0*ratio, 1.0, 100.0);
    glTranslatef(0.0, 0.0, -8.0);

    // Model view
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}



#endif
