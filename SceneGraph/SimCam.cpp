#include "SimCam.h"

static int m_nUsedAttachments = 0;
static int m_nIndex = 0;
        
/////////////////////////////////////////////////////////////////////////////////////////
SimCamMode::SimCamMode( GLSimCam& sc, eSimCamType eCamType ) : m_SimCam( sc ), m_eCamType( eCamType )
{

}

/////////////////////////////////////////////////////////////////////////////////////////
void SimCamMode::PboInit( int format, int type ) 
{
    m_eFormat = format;
    m_eType = type;

    m_pPboIds = new GLuint[1];
    m_nPboIndex = 0;

    int numberOfChannels = NumChannels( m_eFormat );

    int bytesPerChannel = BitsPerChannel( m_eType) / 8;

    m_nDataSize = m_SimCam.ImageWidth() * m_SimCam.ImageHeight() * (numberOfChannels * bytesPerChannel); 

    // Create pixel buffer object
    glGenBuffersARB( 1, m_pPboIds );
    // TODO: cleanup when done and support resizing

    glBindBufferARB( GL_PIXEL_PACK_BUFFER_ARB, m_pPboIds[0] );
    glBufferDataARB( GL_PIXEL_PACK_BUFFER_ARB, m_nDataSize, 0, GL_STREAM_READ_ARB );

    glBindBufferARB( GL_PIXEL_PACK_BUFFER_ARB, 0 );

    // allocate buffer to store frame
    switch (m_eType) {
        case GL_UNSIGNED_BYTE:
	    m_pBuffer = new GLubyte[m_nDataSize / bytesPerChannel];
	    break;
        case GL_FLOAT:
	    m_pBuffer = new GLfloat[m_nDataSize / bytesPerChannel];
	    break; 
        default:
	    m_pBuffer = new GLubyte[m_nDataSize / bytesPerChannel];
	    break;
    }
   
    CheckForGLErrors();
}

/////////////////////////////////////////////////////////////////////////////////////////
void SimCamMode::PboRead()
{
    glReadBuffer( m_eAttachmentIndex );

    // Copy pixels from framebuffer to PBO
    glBindBufferARB( GL_PIXEL_PACK_BUFFER_ARB, m_pPboIds[0] );

    //    glReadPixels( 0, 0, m_SimCam.ImageWidth(), m_SimCam.ImageHeight(), GL_RGB, GL_UNSIGNED_BYTE, 0 );
    glReadPixels( 0, 0, m_SimCam.ImageWidth(), m_SimCam.ImageHeight(), m_eFormat, m_eType, 0 );

    // map the PBO that contains the framebuffer pixels
    glBindBufferARB( GL_PIXEL_PACK_BUFFER_ARB, m_pPboIds[0] );
    GLubyte* src = (GLubyte*)glMapBufferARB( GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY_ARB );

    memcpy( m_pBuffer, src, m_nDataSize );
    glUnmapBufferARB( GL_PIXEL_PACK_BUFFER_ARB );

    glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);
    CheckForGLErrors();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// quick so we can check in matlab
/// TODO: Shouldn't this be done elsewhere?  - JamesII
void _WriteDepthDataToFile( float* vPixels, unsigned int w, unsigned int h )
{
    char sName[1024];
    static int ii = 0;
    snprintf( sName, 1024, "FBO_%04d.m", ii++ );
    FILE* f = fopen( sName, "w" );
    for( unsigned int nRow = 0; nRow < h; nRow++ ){
        for( unsigned int nCol = 0; nCol < w; nCol++ ){
            fprintf( f, "M(%d,%d) = %f;\n", nRow+1, nCol+1, vPixels[nRow*w+nCol] );
        }
    }
    fclose(f);
}

/////////////////////////////////////////////////////////////////////////////////////////
void SimCamMode::Init( bool shader, GLuint sp, GLenum format, GLenum type )
{
    m_bHasShader = shader;
    m_nShaderProgram = sp;
 
    m_eAttachmentIndex = getNextAttachmentIndex();
    m_nColorTextureId = getNextCTId();
    PboInit(format, type);
}

/////////////////////////////////////////////////////////////////////////////////////////
void SimCamMode::RenderToTexture()
{
    m_SimCam.m_pFbo->Begin();
    glDrawBuffer(m_eAttachmentIndex);

    if( m_bHasShader ){ 
        glUseProgram(m_nShaderProgram);
    }
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    m_SimCam.m_pSceneGraph->ApplyDfsVisitor( _ShaderVisitor );

    PboRead();

    if( m_bHasShader ){
        glUseProgram(0);
    }
    m_SimCam.m_pFbo->End();
    CheckForGLErrors();
}

/////////////////////////////////////////////////////////////////////////////////////////
void* SimCamMode::Capture()
{
    return m_pBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////
GLuint SimCamMode::Texture()
{
    return m_SimCam.m_pFbo->m_vColorTextureIds[m_nColorTextureId];
}

/////////////////////////////////////////////////////////////////////////////////////////
/// used to traverse scene graph
void SimCamMode::_ShaderVisitor( GLObject* pObj )
{
    if( pObj->IsPerceptable() ) {
        pObj->draw();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
int SimCamMode::getNextCTId()
{
    int nextIndex = m_nIndex;
    m_nIndex++;
    return nextIndex;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Looks like the limit is 16.
GLenum SimCamMode::getNextAttachmentIndex() 
{
    unsigned int nextAttachment = -1;
    if (m_nUsedAttachments < GL_MAX_COLOR_ATTACHMENTS) { // Not sure if this check works
        switch (m_nUsedAttachments) {
            case 0:
                nextAttachment = GL_COLOR_ATTACHMENT0_EXT;
                break;
            case 1:
                nextAttachment = GL_COLOR_ATTACHMENT1_EXT;
                break;
            case 2:
                nextAttachment = GL_COLOR_ATTACHMENT2_EXT;
                break;
            case 3:
                nextAttachment = GL_COLOR_ATTACHMENT3_EXT;
                break;
            case 4:
                nextAttachment = GL_COLOR_ATTACHMENT4_EXT;
                break;
            case 5:
                nextAttachment = GL_COLOR_ATTACHMENT5_EXT;
                break;
            case 6:
                nextAttachment = GL_COLOR_ATTACHMENT6_EXT;
                break;
            case 7:
                nextAttachment = GL_COLOR_ATTACHMENT7_EXT;
                break;
            case 8:
                nextAttachment = GL_COLOR_ATTACHMENT8_EXT;
                break;
            case 9:
                nextAttachment = GL_COLOR_ATTACHMENT9_EXT;
                break;
            case 10:
                nextAttachment = GL_COLOR_ATTACHMENT10_EXT;
                break;
            case 11:
                nextAttachment = GL_COLOR_ATTACHMENT11_EXT;
                break;
            case 12:
                nextAttachment = GL_COLOR_ATTACHMENT12_EXT;
                break;
            case 13:
                nextAttachment = GL_COLOR_ATTACHMENT13_EXT;
                break;
            case 14:
                nextAttachment = GL_COLOR_ATTACHMENT14_EXT;
                break;
            case 15:
                nextAttachment = GL_COLOR_ATTACHMENT15_EXT;
                break;
        }
        m_nUsedAttachments++;
    } 
    else {
        printf("ERROR: Out of color attachments.\n");
    }

    return nextAttachment;
}

