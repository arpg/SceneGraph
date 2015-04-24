#pragma once

#include "GLHelpersDevil.h"

////////////////////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////////////////////

namespace SceneGraph
{

GLuint LoadGLTextureFromFile(const char* path, size_t length);
GLuint LoadGLTextureFromArray(const unsigned char* data, size_t bytes, const char* extensionHint = 0 );

}

////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////

namespace SceneGraph
{

////////////////////////////////////////////////////////////////////////////
inline GLuint LoadGLTextureFromFile(const char* path, size_t length)
{
    std::string filename(path);
    if(length >= 2 && filename[0] == '/' && filename[1] == '/' ) {
        filename[0] = '.';
    }

    GLuint glTexId = 0;

    std::string extension;
    extension = filename.substr( filename.rfind( "." ) + 1 );


    glTexId = LoadGLTextureFromDevIL(filename);

    return glTexId;
}

////////////////////////////////////////////////////////////////////////////
inline GLuint LoadGLTextureFromArray(const unsigned char* data, size_t bytes, const char* extensionHint )
{
    GLuint glTexId = 0;

    glTexId = LoadGLTextureFromDevIL(data,bytes,extensionHint);

    return glTexId;
}

} // namespace SceneGraph
