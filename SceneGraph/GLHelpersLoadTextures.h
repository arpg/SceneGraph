#pragma once

#include "GLHelpersBoost.h"

#ifdef HAVE_DEVIL
#include "GLHelpersDevil.h"
#endif

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

    glTexId = LoadGLTextureUsingGIL(filename, extension.c_str());

#ifdef HAVE_DEVIL
    if(!glTexId) glTexId = LoadGLTextureFromDevIL(filename);
#endif // HAVE_DEVIL

    return glTexId;
}

////////////////////////////////////////////////////////////////////////////
inline GLuint LoadGLTextureFromArray(const unsigned char* data, size_t bytes, const char* extensionHint )
{
    GLuint glTexId = 0;

    if(!glTexId) glTexId = LoadGLTextureUsingGIL(data,bytes,extensionHint);
#ifdef HAVE_DEVIL
    if(!glTexId) glTexId = LoadGLTextureFromDevIL(data,bytes,extensionHint);
#endif

    return glTexId;
}

} // namespace SceneGraph
