#pragma once

#include "GLHelpers.h"

// Suppress clang unused parameter warnings in boost
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wc++11-narrowing"
#endif

#include <boost/gil/gil_all.hpp>
#ifdef HAVE_PNG
#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL
#include <boost/gil/extension/io/png_io.hpp>
#endif // HAVE_PNG
#ifdef HAVE_JPEG
#include <boost/gil/extension/io/jpeg_io.hpp>
#endif // HAVE_JPEG
#ifdef HAVE_TIFF
#include <boost/gil/extension/io/tiff_io.hpp>
#endif // HAVE_TIFF

// http://code.google.com/p/gil-contributions/
//#include <boost/gil/extension/io_new/png_io_old.hpp>
//#include <boost/gil/extension/io_new/jpeg_io_old.hpp>
//#include <boost/gil/extension/io_new/tiff_io_old.hpp>
//#include <boost/gil/extension/io_new/targa_io_old.hpp>

#ifdef __clang__
#pragma clang diagnostic pop
#endif

////////////////////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////////////////////

namespace SceneGraph
{

void LoadGILImage(boost::gil::rgb8_image_t& runtime_image, std::string filename, const char* extensionHint );

void LoadGILImage(boost::gil::rgb8_image_t& runtime_image, const unsigned char* data, size_t bytes, const char* extensionHint );

GLuint LoadGLTextureUsingGIL(const std::string& filename, const char* extensionHint = 0 );

GLuint LoadGLTextureUsingGIL(const unsigned char* data, size_t bytes, const char* extensionHint = 0 );

}

////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////

namespace SceneGraph
{

inline void LoadGILImage(boost::gil::rgb8_image_t& runtime_image, std::string filename, const char* extensionHint )
{
#ifdef HAVE_PNG
    if(!strcmp(extensionHint,"png")) {
        boost::gil::png_read_and_convert_image(filename, runtime_image);
    }else
#endif // HAVE_PNG
#ifdef HAVE_JPEG
    if(!strcmp(extensionHint,"jpg") || !strcmp(extensionHint,"jpeg")) {
        boost::gil::jpeg_read_and_convert_image(filename, runtime_image);
    }else
#endif // HAVE_JPEG
#ifdef HAVE_TIFF
    if(!strcmp(extensionHint,"tif") ) {
        boost::gil::tiff_read_and_convert_image(filename, runtime_image);
    }else
#endif // HAVE_TIFF

//    // http://code.google.com/p/gil-contributions/
//    if(!strcmp(extensionHint,"tga") ) {
//        boost::gil::targa_read_and_convert_image(filename, runtime_image);
//    }else
        
    {
        // do nothing
    }
}

inline void LoadGILImage(boost::gil::rgb8_image_t& runtime_image, const unsigned char* data, size_t bytes, const char* extensionHint )
{
    const std::string tempfile = "embed.dat";

    std::ofstream of(tempfile.c_str());
    of.write((char*)data, bytes);
    of.close();

    LoadGILImage(runtime_image, tempfile, extensionHint);

    remove(tempfile.c_str());
}

inline GLuint LoadGLTextureUsingGIL(const std::string& filename, const char* extensionHint )
{
    GLuint glTexId = 0;

    boost::gil::rgb8_image_t img;

    LoadGILImage(img, filename, extensionHint);

    if( img.width() > 0 && img.width() > 0 ) {
        unsigned char* data =  boost::gil::interleaved_view_get_raw_data( view( img ) );
        glTexId = LoadGLTexture(img.width(), img.height(), data, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE);
    }

    return glTexId;
}

inline GLuint LoadGLTextureUsingGIL(const unsigned char* data, size_t bytes, const char* extensionHint )
{
    GLuint glTexId = 0;

    boost::gil::rgb8_image_t img;

    LoadGILImage(img, data, bytes, extensionHint);

    if( img.width() > 0 && img.width() > 0 ) {
        unsigned char* data =  boost::gil::interleaved_view_get_raw_data( view( img ) );
        glTexId = LoadGLTexture(img.width(), img.height(), data, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE);
    }

    return glTexId;
}

}

