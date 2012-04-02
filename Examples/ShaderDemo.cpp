#include <SimpleGui/Gui.h> // simple OpenGL scene graph using FLTK 
#include <SimpleGui/GetPot> // for friendy command line parsing
#include <SimpleGui/FBO.h>
#include <SimpleGui/SimCam.h>

using namespace Eigen;

FBO* pFbo;
GLuint g_nDepthShaderProgram;
GLuint g_nNormalShaderProgram;
const int g_nDepthAttachment = GL_COLOR_ATTACHMENT0_EXT;
const int g_nRGBAAttachment = GL_COLOR_ATTACHMENT1_EXT;
const int g_nNormalAttachment = GL_COLOR_ATTACHMENT2_EXT;

Matrix<double, 4, 4, ColMajor> M; // for projection matrix
Matrix<double, 4, 4, ColMajor> T; // for modelview matrix

////////////////////////////////////////////////////////////////////////////////////////////
void ShaderVisitor (GLObject* pObj) 
{
    if( pObj->IsPerceptable() ) {
        pObj->draw();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////
void ShowFBOTextures (GLWindow*, void*)
{
  DrawCamera(pFbo->TexWidth(), pFbo->TexHeight(), pFbo->m_vColorTextureIds[2], T, M);

  DrawTextureAsWindowPercentage(pFbo->m_vColorTextureIds[0], pFbo->TexWidth(), pFbo->TexHeight(),
				0, 0.66, 0.33, 1 );
  DrawBorderAsWindowPercentage( 0, 0.66, 0.33, 1 );

  DrawTextureAsWindowPercentage(pFbo->m_vColorTextureIds[1], pFbo->TexWidth(), pFbo->TexHeight(),
				0.33, 0.66, 0.66, 1);
  DrawBorderAsWindowPercentage(0.33, 0.66, 0.66, 1);

  DrawTextureAsWindowPercentage(pFbo->m_vColorTextureIds[2], pFbo->TexWidth(), pFbo->TexHeight(),
          0.66, 0.66, 1, 1);
  DrawBorderAsWindowPercentage(0.66, 0.66, 1, 1);

}


////////////////////////////////////////////////////////////////////////////////////////////
void ProcessPreRenderShaders (GLWindow* pWin, void*) 
{
  GLint vViewport[4];
  glGetIntegerv(GL_VIEWPORT, vViewport);
  
  GLSceneGraph& sg = pWin->SceneGraph();

  
  glPushAttrib(GL_COLOR_BUFFER_BIT);

  // Setup the camera
  glMatrixMode(GL_PROJECTION);
  glPushMatrix(); // save the projection matrix
  glLoadIdentity(); // our new projection matrix
  gluPerspective(45.0f, (float)vViewport[2]/(float)vViewport[3], 1.0f, 300.0f);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix(); // save the current modelview matrix
  glLoadIdentity(); // our new modelview matrix
  static float f = 0;
  f += 0.1;
  gluLookAt( -20, -20, -10, sin(f), 0, 0, 0, 0, -1);

  glGetDoublev(GL_PROJECTION_MATRIX, M.data());
  glGetDoublev(GL_MODELVIEW_MATRIX, T.data());
  
  glEnable( GL_LIGHTING );
  glEnable( GL_LIGHT0 );

  // render with shaders
  pFbo->Begin();
  glDrawBuffer(g_nDepthAttachment);
  glUseProgram(g_nDepthShaderProgram);
  glClearColor(0.0, 0.0, 0.0, 1); // I like red
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  sg.ApplyDfsVisitor(ShaderVisitor);
  glUseProgram(0);
  pFbo->End();

  // Render the RGBA view
  pFbo->Begin();
  glDrawBuffer(g_nRGBAAttachment);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  sg.ApplyDfsVisitor(ShaderVisitor);
  pFbo->End();

  // Render the Normal view
  pFbo->Begin();
  glDrawBuffer(g_nNormalAttachment);
  glUseProgram(g_nNormalShaderProgram);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  sg.ApplyDfsVisitor(ShaderVisitor);
  glUseProgram(0);
  pFbo->End();


  glMatrixMode(GL_PROJECTION);
  glPopMatrix(); // restore projection matrix
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glPopAttrib();
}

////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char** argv )
{
    GetPot cl( argc, argv );

    pFbo = FBO::Instance();

    std::string sMesh = cl.follow( "Terrain.ac", 1, "-mesh" );

    // init window
    GLWindow* pWin = new GLWindow( 0, 0, 1024, 768, "Simple Gui Demo" );

    // load mesh
    const struct aiScene* pScene;
    struct aiLogStream stream = aiGetPredefinedLogStream( aiDefaultLogStream_STDOUT, NULL );
    aiAttachLogStream( &stream );
    pScene = aiImportFile( sMesh.c_str(), aiProcessPreset_TargetRealtime_MaxQuality );

    GLGrid grid;
    grid.SetPerceptable( false );

    GLMesh mesh;
    mesh.Init( pScene );

    // register objects
    pWin->AddChildToRoot( &mesh );
    pWin->AddChildToRoot( &grid );

    pFbo->Init( 256, 256 );
    
    // Load shader
    if ( LoadShaders( "Depth.vert", "Depth.frag", g_nDepthShaderProgram) == false) {
        fprintf(stderr, "Failed to load the Depth shader.");
        return -1;
    }

    // Load shader
    if ( LoadShaders( "Normals.vert", "Normals.frag", g_nNormalShaderProgram) == false) {
        fprintf(stderr, "Failed to load the Normal shader.");
        return -1;
    }


    // Funcation callbacks
    pWin->AddPreRenderCallback( ProcessPreRenderShaders, NULL );
    pWin->AddPostRenderCallback( ShowFBOTextures, NULL);

    return( pWin->Run() );
}

