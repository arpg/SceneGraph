#include <SimpleGui/Gui.h> // simple OpenGL scene graph using FLTK 

using namespace Eigen;

GLSimCam cam;
GLSimCam cam2;

/////////////////////////////////////////////////////////////////////////////////////////
void ProcessPreRenderShaders (GLWindow* pWin, void*) 
{
    // experiment with changing camera parameters:
//    static float t = 1; t+=0.1;
//    int w = 500*sin(t)+1024; 
//    Eigen::Matrix3d dK;// = Eigen::Matrix3d::Identity();    // computer vision K matrix
//    dK << w,0,50,0,w,50,0,0,1;
//    cam.SetSensorSize( w, w );
//   cam.SetIntrinsics( dK );
 
    static float f;
    Eigen::Matrix4d dPose = GLCart2T( 0, 0,-20,0, -M_PI/2.0 , 0*sin(f+=0.1) ); // initial camera pose
    cam.SetPose( dPose );
    Eigen::Matrix4d dPose2 = GLCart2T( 0, 0,-20,0,-0.5, 0.1*sin(f+=0.1)+0.5 ); // initial camera pose
    cam2.SetPose( dPose2 );

    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glClearColor( 0.0, 0.0, 0.0, 1 );

    cam.Render(); // will render to texture, then copy texture to CPU memory
    cam2.Render();
}

/////////////////////////////////////////////////////////////////////////////////////////
void ShowCameraAndTextures (GLWindow*, void*)
{
    cam.DrawCamera();
    cam2.DrawCamera();

    /// show textures
    DrawTextureAsWindowPercentage( cam.RGBTexture(), cam.ImageWidth(),
				   cam.ImageHeight(), 0, 0.66, 0.33, 1 );
    DrawBorderAsWindowPercentage( 0, 0.66, 0.33, 1 );

    DrawTextureAsWindowPercentage( cam.DepthTexture(), cam.ImageWidth(),
				   cam.ImageHeight(), 0.33, 0.66, 0.66, 1 );
    DrawBorderAsWindowPercentage(0.33, 0.66, 0.66, 1);
     
    DrawTextureAsWindowPercentage( cam2.DepthTexture(), cam2.ImageWidth(),
				   cam2.ImageHeight(), 0.66, 0.66, 1, 1 );
    DrawBorderAsWindowPercentage(0.66, 0.66, 1, 1);

    PushOrtho(200, 200);
    unsigned char* buff = cam2.CaptureDepth();
     
    glDrawPixels(200, 200, GL_RGB, GL_UNSIGNED_BYTE, buff);
    PopOrtho();

    if( gConfig.m_bDebugSimCam ){ // fi debugging SimCam
        float* pDepth = cam.DepthDataPtr();

        glPointSize( 3 );
        glColor4f( 1,0,1,1 );
        glBegin( GL_POINTS );
        int h = cam.ImageHeight();
        int w = cam.ImageWidth();
        for( int v = 0; v < h; v++ ){
            for( int u = 0; u < w; u++ ){

                Eigen::Matrix4d M = cam.GetProjectionMatrixRef();

                Eigen::Matrix3d K;
                K(0,0) = M(0,0); // P00 = 1/tan(fov/2) ==> fx = P00
                K(0,1) = 0.0; // sx
                K(0,2) = w/2.0; // cx
                K(1,0) = 0.0;
                K(1,1) = M(1,1); // fy
                K(1,2) = h/2.0; // cy
                K(2,0) = 0.0;
                K(2,1) = 0.0;
                K(2,2) = 1.0;


                /*
                double near = 10;
                double far = 40; 
                double d = pDepth[v*cam.ImageWidth()+u];
                double z = 2.0*far*near / (far + near - (far - near)*(2.0*d -1));

                // now how to convet to xyz? x/z = u/f
                double x = z*u/200.0;
                double y = z*v/200.0;

                Eigen::Vector4d p;
                p << x,y,z-22,1;
                //            p = cam.GetPoseRef() * p;

                glVertex3dv( p.data() );
                */
            }
        }
        glEnd();
        glPointSize( 1 );
    }
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


/////////////////////////////////////////////////////////////////////////////////
class GLTeapot : public GLObject
{
    public:

    void draw()
    {
        if( gConfig.m_bDebugSimCam ){
            glFrontFace(GL_CW);
            glutSolidTeapot( 8 );
            glFrontFace(GL_CCW);
        }
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

    GLTeapot teapot;

    // register objects
    pWin->AddChildToRoot( &mesh );
    pWin->AddChildToRoot( &teapot );
//    pWin->AddChildToRoot( &grid );

    int w=200;
    int h=200;
    Eigen::Matrix4d dPose = GLCart2T( 1, 1,-4,0,0,M_PI/4 ); // initial camera pose
    Eigen::Matrix3d dK;// = Eigen::Matrix3d::Identity();    // computer vision K matrix
    dK << w,0,50,0,h,50,0,0,1;
    cam.Init( &pWin->SceneGraph(), dPose, dK, w,h );
    cam2.Init( &pWin->SceneGraph(), dPose, dK, w,h );

    _SetupLighting();

    // Funcation callbacks
    pWin->AddPreRenderCallback( ProcessPreRenderShaders, NULL );
    pWin->AddPostRenderCallback( ShowCameraAndTextures, NULL );

    return( pWin->Run() );
}

