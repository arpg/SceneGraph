#ifndef _GLSL_HELPERS_
#define _GLSL_HELPERS_

#include <SceneGraph/GLHelpers.h>
#include <errno.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void PrintShaderInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

    glGetShaderiv( obj, GL_INFO_LOG_LENGTH, &infologLength );

    if (infologLength > 0){
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
        printf("%s\n",infoLog);
        free(infoLog);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void PrintProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

    glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0){
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
        printf("%s\n",infoLog);
        free(infoLog);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void ListUniforms( GLuint p )
{
    printf( "======================\nShader Uniforms:\n");
// enumerate all uniforms
    GLint numUniforms = 0;
    glGetProgramiv( p, GL_ACTIVE_UNIFORMS, &numUniforms );
    GLchar uniformName[64];
    for ( GLint i = 0; i < numUniforms; ++i ){
        GLsizei length;
        GLsizei size;
        GLenum type;
        glGetActiveUniform( p, i, 64, &length, &size, &type, uniformName);
        printf( "%s\n    Length: %3d   Size:  %3d   Type: %3d\n", uniformName, length, size, type );
    }
    printf( "======================\n");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline char *ReadTextFile( const char *fn )
{
    FILE *fp;
    char *content = NULL;
    int count=0;
    if( fn != NULL) {
        fp = fopen(fn,"rt");
        if(fp != NULL) {            
            fseek(fp, 0, SEEK_END);
            count = ftell(fp);
            rewind(fp);
            if (count > 0) {
                content = (char *)malloc(sizeof(char) * (count+1));
                count = fread(content,sizeof(char),count,fp);
                content[count] = '\0';
            }
            fclose(fp);
        }
        else{
            printf( "error opening shader \"%s\" : %s\n", fn, strerror( errno ) );
            exit(-1);
        }
    }
    return content;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline bool LoadShaders( 
        const std::string& sVertexShader,
        const std::string& sFragmentShader,
        GLuint& nShaderProgram
        )
{
    GLuint v = glCreateShader( GL_VERTEX_SHADER );
    GLuint f = glCreateShader( GL_FRAGMENT_SHADER );
    CheckForGLErrors();

    char *vs = ReadTextFile( sVertexShader.c_str() );
    char *fs = ReadTextFile( sFragmentShader.c_str() );
    const char* ff = fs;
    const char* vv = vs;

    glShaderSource( v, 1, &vv, NULL );
    glShaderSource( f, 1, &ff, NULL );
    CheckForGLErrors();

    free(vs);
    free(fs);

    int nResult;
    glCompileShader(v);
    glGetShaderiv( v, GL_COMPILE_STATUS, &nResult );
    if( nResult == GL_FALSE ){
        printf( "ERROR compiling shader.\n");
        return false;
    }
    CheckForGLErrors();

    glCompileShader(f);
    glGetShaderiv( f, GL_COMPILE_STATUS, &nResult );
    if( nResult == GL_FALSE ){
        PrintShaderInfoLog( f );
        PrintProgramInfoLog( nShaderProgram );
        printf( "ERROR compiling shader.\n");
        return false;
    }
    CheckForGLErrors();

    nShaderProgram = glCreateProgram();

    glAttachShader( nShaderProgram,f );
    glAttachShader( nShaderProgram,v );
    CheckForGLErrors();

    glLinkProgram(nShaderProgram);
//    glGetShaderiv( nShaderProgram, GL_LINK_STATUS, &nResult );
    glGetProgramiv( nShaderProgram, GL_LINK_STATUS, &nResult );
    if( nResult == GL_FALSE ){
        PrintProgramInfoLog( nShaderProgram );
        printf( "ERROR linking shader.\n");
        return false;
    }

    CheckForGLErrors();
//    glUseProgram(nShaderProgram);
//    ListUniforms(nShaderProgram);

    return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline bool InitShaders( 
        const std::string& sVertShaderSource, //<  Input:
        const std::string& sFragShaderSource, //< Input:
        GLuint& nShaderProgram //< Output:
        )
{
    int nResult;

    GLuint v = glCreateShader( GL_VERTEX_SHADER );
    const char* vv = sVertShaderSource.c_str();
    glShaderSource( v, 1, &vv, NULL );
    glCompileShader( v );
    glGetShaderiv( v, GL_COMPILE_STATUS, &nResult );
    if( nResult == GL_FALSE ){
        printf( "ERROR compiling shader.\n");
        return false;
    }

    GLuint f = glCreateShader( GL_FRAGMENT_SHADER );
    const char* ff = sFragShaderSource.c_str();
    glShaderSource( f, 1, &ff, NULL );
    glCompileShader( f );
    glGetShaderiv( f, GL_COMPILE_STATUS, &nResult );
    if( nResult == GL_FALSE ){
        printf( "ERROR compiling shader.\n");
        return false;
    }
    CheckForGLErrors();


    nShaderProgram = glCreateProgram();

    glAttachShader( nShaderProgram,f );
    glAttachShader( nShaderProgram,v );
    CheckForGLErrors();

    glLinkProgram(nShaderProgram);
    glGetProgramiv( nShaderProgram, GL_LINK_STATUS, &nResult );
    if( nResult == GL_FALSE ){
        PrintProgramInfoLog( nShaderProgram );
        printf( "ERROR linking shader.\n");
        return false;
    }

    CheckForGLErrors();

    return true;
}


#endif
