#include "SimCam.h"

static const int PBO_COUNT = 2;
static int m_nUsedAttachments = 0;
static int m_nIndex = 0;
        
/////////////////////////////////////////////////////////////////////////////////////////
SimCamMode::SimCamMode( GLSimCam& sc, eSimCamType eCamType ) : m_SimCam( sc ), m_eCamType( eCamType )
{

}

/////////////////////////////////////////////////////////////////////////////////////////
void SimCamMode::PboInit() 
{
    pboIds = new GLuint[PBO_COUNT];
    pboIndex = 0;

    // alloc 4 bytes per pixel so float images work in PboReadDepth...
    data_size = m_SimCam.ImageWidth() * m_SimCam.ImageHeight() * (numberOfChannels+1); 

    // Create pixel buffer objects
    glGenBuffersARB( PBO_COUNT, pboIds );
    // TODO: cleanup when done and support resizing

    // 2 buffers, so that one can be read while the other is being written.
    for( int i = 0; i < PBO_COUNT; i++ ){
        glBindBufferARB( GL_PIXEL_PACK_BUFFER_ARB, pboIds[i] );
        glBufferDataARB( GL_PIXEL_PACK_BUFFER_ARB, data_size, 0, GL_STREAM_READ_ARB );
    }
    glBindBufferARB( GL_PIXEL_PACK_BUFFER_ARB, 0 );

    // allocate buffer to store frame
    buffer = new GLubyte[data_size];
}

/////////////////////////////////////////////////////////////////////////////////////////
void SimCamMode::PboRead()
{
    int nextIndex = 0;

    pboIndex = (pboIndex + 1) % 2;
    nextIndex = (pboIndex + 1) % 2;

    glReadBuffer(attachmentIndex);

    // Copy pixels from framebuffer to PBO
    glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboIds[pboIndex]);
    // TODO: Specify type? maybe as part of the Init function
    glReadPixels(0, 0, m_SimCam.ImageWidth(), m_SimCam.ImageHeight(), GL_RGB, GL_UNSIGNED_BYTE, 0);

    // map the PBO that contains the framebuffer pixels
    // Read from the next buffer since the other is likely being written to still
    glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboIds[nextIndex]);
    GLubyte* src = (GLubyte*)glMapBufferARB(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY_ARB);

    memcpy( buffer, src, data_size );
    glUnmapBufferARB(GL_PIXEL_PACK_BUFFER_ARB);

    glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// quick so we can check in matlab
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
void SimCamMode::PboReadDepth()
{
    int nextIndex = 0;

    pboIndex = (pboIndex + 1) % 2;
    nextIndex = (pboIndex + 1) % 2;

    glReadBuffer( attachmentIndex );

    // Copy pixels from framebuffer to PBO
    glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboIds[pboIndex]);
    // TODO: Specify type? maybe as part of the Init function
    glReadPixels( 0, 0, m_SimCam.ImageWidth(), m_SimCam.ImageHeight(), GL_LUMINANCE, GL_FLOAT, 0 );

    // map the PBO that contains the framebuffer pixels
    // Read from the next buffer since the other is likely being written to still
    glBindBufferARB( GL_PIXEL_PACK_BUFFER_ARB, pboIds[nextIndex] );
    GLubyte* src = (GLubyte*)glMapBufferARB( GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY_ARB );

    unsigned int nNeededSize = m_SimCam.ImageWidth() * m_SimCam.ImageHeight() * sizeof(float);
    if( m_vDepthPixels.size() < nNeededSize ){
        m_vDepthPixels.resize( nNeededSize );
    }
    memcpy( &m_vDepthPixels[0], src, nNeededSize );

    glUnmapBufferARB( GL_PIXEL_PACK_BUFFER_ARB );

    glBindBufferARB( GL_PIXEL_PACK_BUFFER_ARB, 0 );

//    _WriteDepthDataToFile( (float*)buffer, m_SimCam.ImageWidth(), m_SimCam.ImageHeight() );
}


/////////////////////////////////////////////////////////////////////////////////////////
void SimCamMode::Init( bool shader, GLuint sp )
{
    hasShader = shader;
    shaderProgram = sp;
    numberOfChannels = 3;

    attachmentIndex = getNextAttachmentIndex();
    colorTextureId = getNextCTId();
    PboInit();
}

/////////////////////////////////////////////////////////////////////////////////////////
void SimCamMode::RenderToTexture()
{
    m_SimCam.m_pFbo->Begin();
    glDrawBuffer(attachmentIndex);

    if( hasShader ){ 
        glUseProgram(shaderProgram);
    }
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    m_SimCam.m_pSceneGraph->ApplyDfsVisitor( _ShaderVisitor );

    if( m_eCamType == eSimCamDepth ){
        PboReadDepth();
    }
    else{
        PboRead();
    }

    if( hasShader ){
        glUseProgram(0);
    }
    m_SimCam.m_pFbo->End();

}

/////////////////////////////////////////////////////////////////////////////////////////
GLubyte* SimCamMode::Capture()
{
    return buffer;
}

/////////////////////////////////////////////////////////////////////////////////////////
GLuint SimCamMode::Texture()
{
    return m_SimCam.m_pFbo->m_vColorTextureIds[colorTextureId];
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

