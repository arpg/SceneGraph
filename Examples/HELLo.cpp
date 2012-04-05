#include <SimpleGui/Gui.h> // simple OpenGL scene graph using FLTK 
#include <zmq.hpp>

using namespace Eigen;

GLSimCam cam;
zmq::context_t g_Context(1);
zmq::socket_t g_Socket( g_Context, ZMQ_PUB );

const int RGB_CHAN = 3;
const int WIDTH = 200;
const int HEIGHT = 200;

/////////////////////////////////////////////////////////////////////////////////////////
void ProcessPreRenderShaders (GLWindow* pWin, void*) 
{
    static float f;
    Eigen::Matrix4d dPose = GLCart2T( 0, 0,-20,0, -M_PI/2.0 , 0*sin(f+=0.1) ); // initial camera pose
    cam.SetPose( dPose );

    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glClearColor( 0.0, 0.0, 0.0, 1 );
    cam.Render(); // will render to texture, then copy texture to CPU memory
}

/////////////////////////////////////////////////////////////////////////////////////////
void ShowCameraAndTextures (GLWindow*, void*)
{    
    cam.DrawCamera();
    
    /// show textures
    DrawTextureAsWindowPercentage( cam.RGBTexture(), cam.ImageWidth(),
            cam.ImageHeight(), 0, 0.66, 0.33, 1 );
    DrawBorderAsWindowPercentage( 0, 0.66, 0.33, 1 );
    
    GLubyte* buff = cam.CaptureRGB();
    
    PushOrtho(WIDTH, HEIGHT);
    glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, buff);
    PopOrtho();

    zmq::message_t Msg(WIDTH * HEIGHT * RGB_CHAN + 12);
    int numImages = 1;
    char* MsgPtr = (char*)Msg.data();

    // push number of images in message
    memcpy( MsgPtr, &numImages, sizeof(numImages) );
    MsgPtr += sizeof(numImages);

    int ImgType = 16; // Or 24?
    int ImgSize = WIDTH * HEIGHT * RGB_CHAN;

    memcpy( MsgPtr, &WIDTH, sizeof(WIDTH) );
    MsgPtr += sizeof(WIDTH);
    memcpy( MsgPtr, &HEIGHT, sizeof(HEIGHT) );
    MsgPtr += sizeof(HEIGHT);
    memcpy( MsgPtr, &ImgType, sizeof(ImgType) );
    MsgPtr += sizeof(ImgType);

    memcpy( MsgPtr, buff, ImgSize );
    
    g_Socket.send(Msg);
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

    g_Socket.bind("tcp://*:6666");
    // load mesh
    const struct aiScene* pScene;
//    struct aiLogStream stream = aiGetPredefinedLogStream( aiDefaultLogStream_STDOUT, NULL );
//    aiAttachLogStream( &stream );
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

    int w = WIDTH;
    int h = HEIGHT;
    Eigen::Matrix4d dPose = GLCart2T( 1, 1,-4,0,0,M_PI/4 ); // initial camera pose
    Eigen::Matrix3d dK;// = Eigen::Matrix3d::Identity();    // computer vision K matrix
    dK << w,0,50,0,h,50,0,0,1;
    cam.Init( &pWin->SceneGraph(), dPose, dK, w,h );

    _SetupLighting();

    // Funcation callbacks
    pWin->AddPreRenderCallback( ProcessPreRenderShaders, NULL );
    pWin->AddPostRenderCallback( ShowCameraAndTextures, NULL );

    return( pWin->Run() );
}

