#include <SimpleGui/Gui.h> // simple OpenGL scene graph using FLTK 


using namespace Eigen;
using namespace CVarUtils;

GLSimCam cam;
GLSimCam OrthoCam;
GLPointCloud pc;
 
float& fOrthoSize = CreateCVar( "cam.OthroSize", 20.0f, "Size of ortho cam" );

/////////////////////////////////////////////////////////////////////////////////////////
void ProcessPreRenderShaders( GLWindow* pWin, void* ) 
{
    Eigen::Matrix4d dPose = GLCart2T( -30,0,0,0,0,0 ); // initial camera pose
    cam.SetPose( dPose );

    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glClearColor( 0.0, 0.0, 0.0, 1 );

    cam.RenderToTexture(); // will render to texture, then copy texture to CPU memory
}

/////////////////////////////////////////////////////////////////////////////////////////
void ShowCameraAndTextures( GLWindow*, void* )
{
    // copy range data into point cloud
    cam.SetOrtho( fOrthoSize, fOrthoSize ); // force camera to be orhto, 20m by 20m viewing volume 
    cam.DepthTo3D( pc.RangeDataRef() ); 
    pc.draw();

    // show the camera
    cam.DrawCamera();

    /// show textures
    if( cam.HasRGB() ){
        DrawTextureAsWindowPercentage( cam.RGBTexture(), cam.ImageWidth(),
                cam.ImageHeight(), 0, 0.66, 0.33, 1 );
        DrawBorderAsWindowPercentage( 0, 0.66, 0.33, 1 );
    }

    if( cam.HasNormals() ){
        DrawTextureAsWindowPercentage( cam.NormalsTexture(), cam.ImageWidth(),
                cam.ImageHeight(), 0.33, 0.66, 0.66, 1 );
        DrawBorderAsWindowPercentage(0.33, 0.66, 0.66, 1);
    }

    if( cam.HasDepth() ){ 
        DrawTextureAsWindowPercentage( cam.DepthTexture(), cam.ImageWidth(),
                cam.ImageHeight(), 0.66, 0.66, 1, 1 );
        DrawBorderAsWindowPercentage(0.66, 0.66, 1, 1);
    }
}


/////////////////////////////////////////////////////////////////////////////////
class GLTeapot : public GLObject
{
    public:

    void draw()
    {
        glFrontFace(GL_CW);
        glutSolidTeapot( 8 );
        glFrontFace(GL_CCW);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char** argv )
{
    GetPot cl( argc, argv );

    std::string sMesh = cl.follow( "Terrain.ac", 1, "-mesh" );

    // init window
    GLWindow* pWin = new GLWindow( 0, 0, 1024, 768, "Simple Gui Demo" );

    // load mesh
    const struct aiScene* pScene;
     pScene = aiImportFile( sMesh.c_str(),
             aiProcess_Triangulate |
             aiProcess_GenNormals 
             );

    GLGrid grid;
    grid.SetPerceptable( false );

    GLMesh mesh;
    mesh.Init( pScene );

    GLTeapot teapot;

    // register objects
    pWin->AddChildToRoot( &mesh );
    pWin->AddChildToRoot( &teapot );
    pWin->AddChildToRoot( &grid );

    int w = 128;
    int h = 128;
    Eigen::Matrix4d dPose = GLCart2T( -20, 0,0,0,0,0 ); // initial camera pose
    Eigen::Matrix3d dK;   // computer vision K matrix
    dK << w,0,w/2,0,h,h/2,0,0,1;

//    cam.Init( &pWin->SceneGraph(), dPose, dK, w,h, eSimCamRGB | eSimCamDepth );
    cam.Init( &pWin->SceneGraph(), dPose, dK, w,h, eSimCamRGB | eSimCamDepth );
    cam.SetOrtho( fOrthoSize, fOrthoSize ); // force camera to be orhto, 20m by 20m viewing volume 

    glEnable( GL_LIGHT0 );                                         // activate light0
    glEnable( GL_LIGHTING );                                       // enable lighting

    pWin->LookAt( -70, -70, -50, 0,0,0, 0,0,-1 );

    // add our callbacks
    pWin->AddPreRenderCallback( ProcessPreRenderShaders, NULL );
    pWin->AddPostRenderCallback( ShowCameraAndTextures, NULL );

    return( pWin->Run() );
}

