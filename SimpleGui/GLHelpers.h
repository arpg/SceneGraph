/*
 *  \file GLHelpers.h
 *
 * $Id$
 */ 
 
#ifndef _GL_HELPERS_H_
#define _GL_HELPERS_H_


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

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <Eigen/Core>
#include <Eigen/LU>

#define CheckForGLErrors() _CheckForGLErrors( __FILE__, __LINE__ );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///  If a GL error has occured, this function outputs "msg" and the
//   programme exits. To avoid exiting @see WarnForGLErrors.
inline void _CheckForGLErrors( const char *sFile = NULL, const int nLine = -1 )
{
    GLenum glError = glGetError();
    if( glError != GL_NO_ERROR ) {
        fprintf( stderr, "ERROR: %s -- %s line %d\n", 
                (char*)gluErrorString(glError), sFile, nLine );
        //FIXME: On my box, glError is not totally free of error.
        // there's probably something wrong with one of the glEnable(...) in GLWindow.cpp
        // so I temporarily comment out the exit() call here. 
        // exit( -1 );
    }
}


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
/// If a GL error has occured, this function outputs "msg" and  automatically sets 
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
    glOrtho(0, nWidth, nHeight, 0, -1, 1); // left, right, top, bottom, near, far
    glMatrixMode( GL_MODELVIEW );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Set projection matrix
inline void PopOrtho()
{
    glBindTexture( GL_TEXTURE_RECTANGLE_ARB, 0 );
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Set projection matrix
inline void OrthoQuad( 
        const int nTexWidth,    //< Input:
        const int nTexHeight,   //< Input:
        const int nTop,      //< Input:
        const int nLeft,     //< Input:
        const int nBottom,   //< Input:
        const int nRight     //< Input:
        )
{
    /*
    glBegin( GL_QUADS );
    glTexCoord2f(    0.0,    0.0  ); glVertex3f( nLeft,  nBottom, 1 );
    glTexCoord2f(    0.0, nTexHeight ); glVertex3f( nLeft,  nTop, 1 );
    glTexCoord2f( nTexWidth, nTexHeight ); glVertex3f( nRight, nTop, 1 );
    glTexCoord2f( nTexWidth,    0.0  ); glVertex3f( nRight, nBottom, 1 );
    glEnd();
    */

    glNormal3f( -1,0,0 );
    glBegin( GL_QUADS );
    glTexCoord2f(       0.0,       0.0  ); glVertex3f(  nLeft, nBottom, 0 );
    glTexCoord2f( nTexWidth,       0.0  ); glVertex3f( nRight, nBottom, 0 );
    glTexCoord2f( nTexWidth, nTexHeight ); glVertex3f( nRight,    nTop, 0 );
    glTexCoord2f(       0.0, nTexHeight ); glVertex3f(  nLeft,    nTop, 0 );
    glEnd();


}

////////////////////////////////////////////////////////////////////////////////////////////////////
inline void DrawBorderAsWindowPercentage(
        const float fTop,
        const float fLeft,
        const float fBottom,
        const float fRight
        )
{
    GLint vViewport[4];
    glGetIntegerv( GL_VIEWPORT, vViewport );

    PushOrtho( vViewport[2], vViewport[3] );
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glBegin( GL_QUADS );
    glVertex3f(  fLeft*vViewport[2], fBottom*vViewport[3], 0 );
    glVertex3f( fRight*vViewport[2], fBottom*vViewport[3], 0 );
    glVertex3f( fRight*vViewport[2],    fTop*vViewport[3], 0 );
    glVertex3f(  fLeft*vViewport[2],    fTop*vViewport[3], 0 );
    glEnd();
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    PopOrtho();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
inline void DrawTextureAsWindowPercentage(
        const unsigned int nTexId,      //< Input:
        const unsigned int nTexWidth, //< Input:
        const unsigned int nTexHeight,//< Input:
        const float fTop,
        const float fLeft,
        const float fBottom,
        const float fRight
    )
{
    glDisable( GL_LIGHTING );
    glColor4f( 1,1,1,1 ); // only scoop up texture colors

    GLint vViewport[4];
    glGetIntegerv( GL_VIEWPORT, vViewport );

    glEnable(GL_TEXTURE_RECTANGLE_ARB);
    glBindTexture( GL_TEXTURE_RECTANGLE_ARB, nTexId );

    PushOrtho( vViewport[2], vViewport[3] );
    OrthoQuad( nTexWidth, nTexHeight, 
            fTop*vViewport[3], fLeft*vViewport[2], 
            fBottom*vViewport[3], fRight*vViewport[2] ); 

    PopOrtho();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
inline void DrawTexture(
        const unsigned int nTexId,      //< Input:
        const unsigned int nTexWidth, //< Input:
        const unsigned int nTexHeight,//< Input:
        const unsigned int nTop,        //< Input:
        const unsigned int nLeft,       //< Input:
        const unsigned int nBottom,     //< Input:
        const unsigned int nRight       //< Input:
        )
{
    // NB DECAL ignores lighting
//    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
    glDisable( GL_LIGHTING );
    glColor4f( 1,1,1,1 ); // only scoop up texture colors

    GLint vViewport[4];
    glGetIntegerv( GL_VIEWPORT, vViewport );

    glEnable(GL_TEXTURE_RECTANGLE_ARB);
    glBindTexture( GL_TEXTURE_RECTANGLE_ARB, nTexId );

    PushOrtho( vViewport[2], vViewport[3] );
    OrthoQuad( nTexWidth, nTexHeight, nTop, nLeft, nBottom, nRight ); 
    PopOrtho();
}

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
    }
    switch ( nFormat ) {
        case GL_LUMINANCE: break;
        case GL_RGB: nVal*=3; break;
        case GL_RGBA: nVal*=4; break;
        default: fprintf ( stderr, "GLBytesPerPixel() -- unsupported format\n" ); return 0;
    }
    return nVal;
}


//////////////////////////////////////////////////////////////////////////////
// read from opengl buffer into our own vector
inline void ReadDepthPixels( 
        std::vector<float>& vPixels,
        int nWidth,
        int nHeight,
        int nFormat,
        int nType,
        bool bFlip = true
        )
{
    unsigned int nBpp = GLBytesPerPixel( nFormat, nType );
    if( vPixels.size() < nWidth*nBpp*nHeight ){
        vPixels.resize( nWidth*nBpp*nHeight );
    }
    char* pPixelData = (char*)&vPixels[0];

    glReadPixels( 0, 0, nWidth, nHeight, nFormat, nType, pPixelData );

    if( bFlip ){
        int inc = nBpp*nWidth;
        char* pSwap = (char*)malloc( inc );
        char* pTopDown = pPixelData;
        char* pBottomUp = &pPixelData[ nWidth*nBpp*nHeight ];
        for( ; pTopDown < pBottomUp; pTopDown += inc, pBottomUp -= inc ){
            memcpy( pSwap, pTopDown, inc ); 
            memcpy( pTopDown, pBottomUp, inc ); 
            memcpy( pBottomUp, pSwap, inc ); 
        }
        free( pSwap );
    }
}


//////////////////////////////////////////////////////////////////////////////
inline void CheckFBOStatus()
{
    //Does the GPU support current FBO configuration?
    GLenum status;
    status = glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );
    switch(status) {
        case GL_FRAMEBUFFER_COMPLETE_EXT:
        //    std::cerr << "The framebuffer is complete and valid for rendering.\n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
            std::cerr << "One or more attachment points are not framebuffer attachment complete.\n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            std::cerr << "There are no attachments.\n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            std::cerr << "Attachments are of different size. All attachments must have the same width and height.\n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
            std::cerr << "The color attachments have different format. All color attachments must have the same format.\n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            std::cerr << "An attachment point referenced by glDrawBuffers() doesn’t have an attachment.\n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            std::cerr << "The attachment point referenced by glReadBuffers() doesn’t have an attachment.\n";
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            std::cerr << "This particular FBO configuration is not supported by the implementation.\n";
            break;
    }
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
    Perspective( 60.0, 1.0*ratio, 1.0, 1000.0 );

    // Model view
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
inline Eigen::Vector4d Vec4( double a, double b, double c, double d )
{
        Eigen::Vector4d tmp;
            tmp << a, b, c, d;
                return tmp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
inline void DrawCamera(
        int nTexWidth,
        int nTexHeight,
        int nTexId,
        Eigen::Matrix4d dModelViewMatrix,
        Eigen::Matrix4d dProjectionMatrix
        )
{
    // OK 
    Eigen::Matrix4d M = dProjectionMatrix.inverse();
    Eigen::Matrix4d T = dModelViewMatrix.inverse();

    Eigen::Vector4d lbn = T*M*Vec4( -1,-1,-1, 1 );  lbn/=lbn[3];
    Eigen::Vector4d rbn = T*M*Vec4(  1,-1,-1, 1 );  rbn/=rbn[3];
    Eigen::Vector4d ltn = T*M*Vec4( -1, 1,-1, 1 );  ltn/=ltn[3];
    Eigen::Vector4d rtn = T*M*Vec4(  1, 1,-1, 1 );  rtn/=rtn[3];

    Eigen::Vector4d lbf = T*M*Vec4( -1,-1, 1, 1 );  lbf/=lbf[3];
    Eigen::Vector4d rbf = T*M*Vec4(  1,-1, 1, 1 );  rbf/=rbf[3];
    Eigen::Vector4d ltf = T*M*Vec4( -1, 1, 1, 1 );  ltf/=ltf[3];
    Eigen::Vector4d rtf = T*M*Vec4(  1, 1, 1, 1 );  rtf/=rtf[3];

    glColor4f( 1,1,1,1 );
    glColor4f( 1,1,1,1 ); // so GL_MODULATE will just light the texture

    /// Draw texture
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
    glEnable( GL_TEXTURE_RECTANGLE_ARB );
    glBindTexture( GL_TEXTURE_RECTANGLE_ARB, nTexId );

    glEnable(GL_DEPTH_TEST);
    glEnable( GL_LIGHTING );
    glEnable(GL_LIGHT0);
    glDisable(GL_CULL_FACE );

    glBegin( GL_QUADS );
    glNormal3f( -1,0,0 );
    glTexCoord2f(       0.0,       0.0  ); glVertex3dv( lbn.data() );
    glTexCoord2f( nTexWidth,       0.0  ); glVertex3dv( rbn.data() );
    glTexCoord2f( nTexWidth, nTexHeight ); glVertex3dv( rtn.data() );
    glTexCoord2f(       0.0, nTexHeight ); glVertex3dv( ltn.data() );
    glEnd();
    glBindTexture( GL_TEXTURE_RECTANGLE_ARB, 0 );
    glDisable(GL_TEXTURE_RECTANGLE_ARB);
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    // draw frustrum
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glBegin( GL_QUADS );

    // near
    glVertex3dv( lbn.data() );
    glVertex3dv( rbn.data() );
    glVertex3dv( rtn.data() );
    glVertex3dv( ltn.data() );

    // far
    glVertex3dv( rbf.data() );
    glVertex3dv( lbf.data() );
    glVertex3dv( ltf.data() );
    glVertex3dv( rtf.data() );

    // left
    glVertex3dv( lbf.data() );
    glVertex3dv( lbn.data() );
    glVertex3dv( ltn.data() );
    glVertex3dv( ltf.data() );

    // right
    glVertex3dv( rbn.data() );
    glVertex3dv( rbf.data() );
    glVertex3dv( rtf.data() );
    glVertex3dv( rtn.data() );

    // top
    glVertex3dv( ltn.data() );
    glVertex3dv( rtn.data() );
    glVertex3dv( rtf.data() );
    glVertex3dv( ltf.data() );

    // bottom
    glVertex3dv( lbn.data() );
    glVertex3dv( rbn.data() );
    glVertex3dv( rbf.data() );
    glVertex3dv( lbf.data() );

    glEnd();
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

//    glDisable( GL_DEPTH_TEST );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    //    glBlendFunc( GL_SRC_ALPHA_SATURATE,  GL_ONE );
    glEnable( GL_BLEND );
    glColor4f( 1,1,1,0.1 );
    glBegin( GL_QUADS );

    // near
    glVertex3dv( lbn.data() );
    glVertex3dv( rbn.data() );
    glVertex3dv( rtn.data() );
    glVertex3dv( ltn.data() );


    // far
    glVertex3dv( rbf.data() );
    glVertex3dv( lbf.data() );
    glVertex3dv( ltf.data() );
    glVertex3dv( rtf.data() );

    // left
    glVertex3dv( lbf.data() );
    glVertex3dv( lbn.data() );
    glVertex3dv( ltn.data() );
    glVertex3dv( ltf.data() );

    // right
    glVertex3dv( rbn.data() );
    glVertex3dv( rbf.data() );
    glVertex3dv( rtf.data() );
    glVertex3dv( rtn.data() );

    // top
    glVertex3dv( ltn.data() );
    glVertex3dv( rtn.data() );
    glVertex3dv( rtf.data() );
    glVertex3dv( ltf.data() );

    // bottom
    glVertex3dv( lbn.data() );
    glVertex3dv( rbn.data() );
    glVertex3dv( rbf.data() );
    glVertex3dv( lbf.data() );

    glEnd();
    glDisable( GL_BLEND );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );

}



#endif
