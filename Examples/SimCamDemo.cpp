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
    Eigen::Matrix3d dK;// = Eigen::Matrix3d::Identity();    // computer vision K matrix
    dK << w,0,50,0,w,50,0,0,1;
 
    f += 0.1;
    Eigen::Matrix4d dPose = GLCart2T( 0, 0,-20,0,-0.5, 0.1*sin(f)+0.5 ); // initial camera pose
//    Eigen::Matrix4d dPose = GLCart2T( 0, 0,-20,0,0,0 ); // initial camera pose
    cam.SetPose( dPose );

    // experiment with changing camera parameters:
//    cam.SetSensorSize( w, w );
//   cam.SetIntrinsics( dK );

    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glClearColor(0.0, 0.0, 0.0, 1);
    cam.Render();

//    RightCam.Render();
}

/////////////////////////////////////////////////////////////////////////////////////////
void ShowCameraAndTextures (GLWindow*, void*)
{
    cam.DrawCamera();

    /// show textures
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



/////////////////////////////////////////////////////////////////////////////////
void _SetupLighting()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt( 10,10,-10, 0,0,0, 0,0,-1 );

    /////
    GLfloat ambientLight[]  ={ 0.1, 0.1, 0.1, 1.0 };             // set ambient light parameters
    glLightfv( GL_LIGHT0, GL_AMBIENT, ambientLight );

    GLfloat diffuseLight[] = { 0.8, 0.8, 0.8, 1.0 };             // set diffuse light parameters
    glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuseLight );

    GLfloat specularLight[] = { 0.5, 0.5, 0.5, 1.0 };                   // set specular light parameters
    glLightfv(GL_LIGHT0,GL_SPECULAR,specularLight);

    GLfloat lightPos[] = { 10.0, 10.0, -100.0, 1.0};                 // set light position
    glLightfv( GL_LIGHT0, GL_POSITION, lightPos );

    GLfloat specularReflection[] = { 1.0, 1.0, 1.0, 1.0 };              // set specularity
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specularReflection );
    glMateriali( GL_FRONT_AND_BACK, GL_SHININESS, 128 );
    glEnable( GL_LIGHT0 );                                         // activate light0
    glEnable( GL_LIGHTING );                                       // enable lighting
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, ambientLight );        // set light model
    glEnable( GL_COLOR_MATERIAL );                                 // activate material
    glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
//    glEnable( GL_NORMALIZE );                                   // normalize normal vectors

//    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

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
    struct aiLogStream stream = aiGetPredefinedLogStream( aiDefaultLogStream_STDOUT, NULL );
    aiAttachLogStream( &stream );
     pScene = aiImportFile( sMesh.c_str(),
             aiProcess_Triangulate |
             aiProcess_GenNormals 
             );

    GLGrid grid;
    grid.SetPerceptable( false );

    GLMesh mesh;
    mesh.Init( pScene );

    // register objects
    pWin->AddChildToRoot( &mesh );
//    pWin->AddChildToRoot( &grid );

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

    _SetupLighting();

    // Funcation callbacks
    pWin->AddPreRenderCallback( ProcessPreRenderShaders, NULL );
    pWin->AddPostRenderCallback( ShowCameraAndTextures, NULL );

    return( pWin->Run() );
}

