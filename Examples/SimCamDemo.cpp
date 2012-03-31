#include <SimpleGui/Gui.h> // simple OpenGL scene graph using FLTK 
#include <SimpleGui/GetPot> // for friendy command line parsing
#include <SimpleGui/FBO.h>
#include <SimpleGui/SimCam.h>

using namespace Eigen;


GLSimCam cam;
GLSimCam RightCam;

/////////////////////////////////////////////////////////////////////////////////////////
void ProcessPreRenderShaders (GLWindow* pWin, void*) 
{
    static float f;
    static float t = 1; t+=0.1;
    int w = 500*sin(t)+1024; 

    f += 0.0;
    Eigen::Matrix4d dPose = GLCart2T( 1, 1,-4,0,-0.5, sin(f)+0.5 ); // initial camera pose
    cam.SetPose( dPose );
//    cam.SetSensorSize( w, w );
    Eigen::Matrix3d dK;// = Eigen::Matrix3d::Identity();    // computer vision K matrix
    dK << w,0,50,0,w,50,0,0,1;
    cam.SetIntrinsics( dK );

    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glClearColor(0.0, 0.0, 0.0, 1);
    cam.Render();

//    RightCam.Render();
}

/////////////////////////////////////////////////////////////////////////////////////////
void ShowFBOTextures (GLWindow*, void*)
{
    DrawCamera( cam.ImageWidth(), cam.ImageHeight(), cam.RGBTexture(),
            cam.GetModelViewMatrixRef(), cam.GetProjectionMatrixRef() );

    DrawTextureAsWindowPercentage( cam.RGBTexture(), cam.ImageWidth(),
            cam.ImageHeight(), 0, 0.66, 0.33, 1 );
    DrawBorderAsWindowPercentage( 0, 0.66, 0.33, 1 );

  DrawTextureAsWindowPercentage( cam.DepthTexture(), cam.ImageWidth(),
          cam.ImageHeight(), 0.33, 0.66, 0.66, 1 );
  DrawBorderAsWindowPercentage(0.33, 0.66, 0.66, 1);

  DrawTextureAsWindowPercentage( cam.NormalTexture(), cam.ImageWidth(),
          cam.ImageHeight(), 0.66, 0.66, 1, 1 );
  DrawBorderAsWindowPercentage(0.66, 0.66, 1, 1);
}


/////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char** argv )
{
    GetPot cl( argc, argv );

    std::string sMesh = cl.follow( "Terrain.ac", 1, "-mesh" );

    // init window
    GLWindow* pWin = new GLWindow( 0, 0, 1024, 768, "Simple Gui Demo" );

    // load mesh
    const struct aiScene* pScene;
    pScene = aiImportFile( sMesh.c_str(), aiProcessPreset_TargetRealtime_MaxQuality );

    GLGrid grid;
    grid.SetPerceptable( false );

    GLMesh mesh;
    mesh.Init( pScene );

    // register objects
    pWin->AddChildToRoot( &mesh );
    pWin->AddChildToRoot( &grid );

    int w=1024;
    int h=768;
    Eigen::Matrix4d dPose = GLCart2T( 1, 1,-4,0,0,M_PI/4 ); // initial camera pose
    Eigen::Matrix3d dK;// = Eigen::Matrix3d::Identity();    // computer vision K matrix
    dK << w,0,50,0,h,50,0,0,1;
    cam.Init( &pWin->SceneGraph(), dPose, dK, w,h );
    CheckForGLErrors();

    Eigen::Matrix4d dRightPose = GLCart2T( 1, 2,-4,0,0,M_PI/4 ); // initial camera pose
    RightCam.Init( &pWin->SceneGraph(), dRightPose, dK, w,h );
    CheckForGLErrors();

    // Funcation callbacks
    pWin->AddPreRenderCallback( ProcessPreRenderShaders, NULL );
    pWin->AddPostRenderCallback( ShowFBOTextures, NULL );

    return( pWin->Run() );
}

