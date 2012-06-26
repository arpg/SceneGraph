#include <SimpleGui/Gui.h> // simple OpenGL scene graph using FLTK 

using namespace Eigen;

GLSimCam cam;

/////////////////////////////////////////////////////////////////////////////////////////
void ProcessPreRenderShaders (GLWindow*, void*) 
{
    static float f;
    Eigen::Matrix4d dPose = GLCart2T( 0, 0,-20,0, -M_PI/2.0 , 0*sin(f+=0.1) ); // initial camera pose
    cam.SetPose( dPose );

    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glClearColor( 0.0, 0.0, 0.0, 1 );
    cam.RenderToTexture(); // will render to texture, then copy texture to CPU memory
}

/////////////////////////////////////////////////////////////////////////////////////////
void ShowCameraAndTextures (GLWindow*, void*)
{    
    cam.DrawCamera();
    
    /// show textures
    if ( cam.HasDepth() ) {
        DrawTextureAsWindowPercentage( cam.DepthTexture(), cam.ImageWidth(),
				       cam.ImageHeight(), 0, 0.66, 0.33, 1 );
	DrawBorderAsWindowPercentage( 0, 0.66, 0.33, 1 );
    }
   
    if ( cam.HasRGB() ) {
        std::vector<unsigned char> vPixels; // will be filled/written by CaptureRGB
	cam.CaptureRGB( vPixels );
	PushOrtho( 200, 200 );
	glDrawPixels( 200, 200, GL_RGB, GL_UNSIGNED_BYTE, &vPixels[0] );
	PopOrtho();
    }

    CheckForGLErrors();
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
    GLWindow* pWin = new GLWindow( 0, 0, 1024, 768, "PBO Test" );

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

    // register objects
    pWin->AddChildToRoot( &mesh );

    int w=200;
    int h=200;
    Eigen::Matrix4d dPose = GLCart2T( 1, 1,-4,0,0,M_PI/4 ); // initial camera pose
    Eigen::Matrix3d dK;// = Eigen::Matrix3d::Identity();    // computer vision K matrix
    dK << w,0,50,0,h,50,0,0,1;
    cam.Init( &pWin->SceneGraph(), dPose, dK, w,h, eSimCamDepth | eSimCamRGB);

    _SetupLighting();

    // Funcation callbacks
    pWin->AddPreRenderCallback( ProcessPreRenderShaders, NULL );
    pWin->AddPostRenderCallback( ShowCameraAndTextures, NULL );

    return( pWin->Run() );
}

