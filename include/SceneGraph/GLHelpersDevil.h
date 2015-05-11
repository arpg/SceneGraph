#pragma once

#include <IL/il.h>
#include <IL/ilu.h>

////////////////////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////////////////////
namespace SceneGraph
{

GLuint LoadGLTextureFromDevIL(ILuint ilTexId);

GLuint LoadGLTextureFromDevIL(const std::string filename);

GLuint LoadGLTextureFromDevIL(const unsigned char* data, size_t bytes, const char* extensionHint = 0 );

}

////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////

namespace SceneGraph
{

////////////////////////////////////////////////////////////////////////////
inline GLuint LoadGLTextureFromDevIL(ILuint ilTexId)
{
    ilBindImage(ilTexId);

    ILinfo ilImageInfo;
    iluGetImageInfo(&ilImageInfo);

    if (ilImageInfo.Origin == IL_ORIGIN_UPPER_LEFT) {
        iluFlipImage();
    }

    if ( !ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE) ) {
        ILenum ilError = ilGetError();
        std::cerr << "Unable to decode image, DevIL: " << ilError << " - " << iluErrorString(ilError) << std::endl;
        return 0;
    }

    GLuint glTexId = 0;
    glGenTextures(1, &glTexId);
    glBindTexture(GL_TEXTURE_2D, glTexId);
    // load Mipmaps instead of single texture
    glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),
                 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData() );
//            glGenerateMipmap( GL_TEXTURE_2D );
//    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),
//                ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData() );
    return glTexId;
}

////////////////////////////////////////////////////////////////////////////
inline GLuint LoadGLTextureFromDevIL(const std::string filename)
{
    GLuint glTexId = 0;

    ILuint ilTexId;
    ilGenImages(1, &ilTexId);
    ilBindImage(ilTexId);

    if( ilLoadImage(filename.c_str() ) ) {
        glTexId = LoadGLTextureFromDevIL(ilTexId);
    }else{
        std::cerr << "Failed to load texture file '" << filename << "'" << std::endl;
    }

    ilDeleteImages(1, &ilTexId);
    return glTexId;
}

////////////////////////////////////////////////////////////////////////////
inline GLuint LoadGLTextureFromDevIL(const unsigned char* data, size_t bytes, const char* extensionHint )
{
    GLuint glTexId = 0;

    ILuint ilTexId;
    ilGenImages(1, &ilTexId);
    ilBindImage(ilTexId);

    ILenum filetype = IL_TYPE_UNKNOWN;

    // Guess filetype by data
    if (filetype == IL_TYPE_UNKNOWN) {
        filetype = ilDetermineTypeL(data, bytes);
    }

    // Guess filetype by extension
    if( filetype == IL_TYPE_UNKNOWN && extensionHint != 0 ) {
        const std::string sExt(extensionHint);
        const std::string sDummy = std::string("dummy.") + sExt;
        filetype = ilDetermineType(sDummy.c_str());
    }

    // Some extra guesses that seem to be missed in devIL
    if( filetype == IL_TYPE_UNKNOWN ) {
        if( !strcmp(extensionHint, "bmp") ) {
            filetype = IL_TGA;
        }
    }

    // Load image to ilTexId us devIL
    if( ilLoadL(filetype, data, bytes) ) {
        glTexId = LoadGLTextureFromDevIL(ilTexId);
    }else{
        ILenum ilError = ilGetError();
        std::cerr << "Failed to Load embedded texture, DevIL: " << ilError << " - " << iluErrorString(ilError) << std::endl;
    }

    ilDeleteImages(1, &ilTexId);

    return glTexId;
}

} // namespace SceneGraph
