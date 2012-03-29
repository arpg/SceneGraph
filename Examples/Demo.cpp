#include <SimpleGui/Gui.h> // simple OpenGL scene graph using FLTK 
#include <SimpleGui/GetPot> // for friendy command line parsing
#include <SimpleGui/FBO.h>

using namespace Eigen;

FBO fbo;
GLuint g_nDepthShaderProgram;
const int g_nDepthAttachment = GL_COLOR_ATTACHMENT0_EXT;
const int g_nRGBAAttachment = GL_COLOR_ATTACHMENT1_EXT;

Matrix<double, 4, 4, ColMajor> M; // for projection matrix
Matrix<double, 4, 4, ColMajor> T; // for modelview matrix

void ShaderVisitor (GLObject* pObj) {
  pObj->draw();
}

void Update(GLWindow*, void*) {
  
}  

void ShowFBOTextures (GLWindow*, void*)
{
  DrawCamera(fbo.TexWidth(), fbo.TexHeight(), fbo.m_vColorTextureIds[2], T, M);

  DrawTextureAsWindowPercentage(fbo.m_vColorTextureIds[0], fbo.TexWidth(), fbo.TexHeight(),
				0, 0.66, 0.33, 1 );
  DrawBorderAsWindowPercentage( 0, 0.66, 0.33, 1 );

  DrawTextureAsWindowPercentage(fbo.m_vColorTextureIds[1], fbo.TexWidth(), fbo.TexHeight(),
				0.33, 0.66, 0.66, 1);
  DrawBorderAsWindowPercentage(0.33, 0.66, 0.66, 1);
}

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
  gluLookAt( -1, -4, -2, sin(f), 0, 0, 0, 0, -1);

  glGetDoublev(GL_PROJECTION_MATRIX, M.data());
  glGetDoublev(GL_MODELVIEW_MATRIX, T.data());

  // render with shaders
  fbo.Begin();
  glDrawBuffer(g_nDepthAttachment);
  glUseProgram(g_nDepthShaderProgram);
  glClearColor(1.0, 0.0, 0.0, 1); // I like red
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glColor4f(1, 1, 1, 1);
  sg.ApplyDfsVisitor(ShaderVisitor);
  glUseProgram(0);
  fbo.End();

  // Render the RGBA view
  fbo.Begin();
  glDrawBuffer(g_nRGBAAttachment);
  glClearColor(0, 0, 0.5, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glColor4f(1, 1, 1, 1);
  sg.ApplyDfsVisitor(ShaderVisitor);
  fbo.End();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix(); // restore projection matrix
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glPopAttrib();
}
int main( int argc, char** argv )
{
  GetPot cl( argc, argv );

  std::string sMesh = cl.follow( "Terrain.ac", 1, "-mesh" );

  // init window
  GLWindow* pWin = new GLWindow( 0, 0, 1024, 768, "Simple Gui Demo" );

  // load mesh
  const struct aiScene* pScene;
  struct aiLogStream stream = aiGetPredefinedLogStream( aiDefaultLogStream_STDOUT, NULL );
  aiAttachLogStream( &stream );
  pScene = aiImportFile( sMesh.c_str(), aiProcessPreset_TargetRealtime_MaxQuality );

  GLMesh mesh;
  GLGrid grid;

  mesh.Init( pScene );


  // register objects
  pWin->AddChildToRoot( &mesh );
  pWin->AddChildToRoot( &grid );

  fbo.Init( 256, 256 );

  // Load shader
  if ( LoadShaders( "Depth.vert", "Depth.frag", g_nDepthShaderProgram) == false) {
    fprintf(stderr, "Failed to load the Depth shader.");
    return -1;
  }

  // Funcation callbacks
  pWin->AddPreRenderCallback( Update, NULL );
  pWin->AddPreRenderCallback( ProcessPreRenderShaders, NULL );
  pWin->AddPostRenderCallback( ShowFBOTextures, NULL);

  return( pWin->Run() );
}

