#include <SimpleGui/Gui.h> // simple OpenGL scene graph using FLTK 

#include "GLHeightMap.h"
#include "PeaksHeightMap.h" 

using namespace Eigen;
using namespace CVarUtils;

GLSimCam cam;
GLSimCam OrthoCam;
GLPointCloud PointCloud;

Eigen::Matrix4d g_dPose = GLCart2T( -40,0,-5,0,0,0 ); // initial camera pose
float g_fTurnrate = 0;
float g_fSpeed = 0;


float& fOrthoSize = CreateCVar( "cam.OthroSize", 20.0f, "Size of ortho cam" );
bool&  bOrtho = CreateCVar( "cam.OthroOn", false, "Use orthographic projection" );

/////////////////////////////////////////////////////////////////////////////////////////
/// example of how to handle fltk events in a sub-class
class GuiWindow: public GLWindow
{
    public:
        GuiWindow(int x,int y,int w,int h,const char *l=0 ) : GLWindow(x,y,w,h,l) {}

        virtual int handle( int e )
        {
            if( e == FL_KEYBOARD && !m_Console.IsOpen()  ){
                switch( Fl::event_key() ) {
                    case 'a': case 'A': 
                        g_fTurnrate -= 0.01;
                        break;
                    case 's': case 'S':
                        g_fSpeed -= 0.2;
                        break;
                    case 'd': case 'D': 
                        g_fTurnrate += 0.01;
                        break;
                    case 'w': case 'W':
                        g_fSpeed += 0.2;
                        break;
                    case ' ':
                        g_fSpeed = 0;
                        g_fTurnrate = 0;
                        break;
                }
            }
            return SimpleDefaultEventHandler( e ); 
        }
};

/////////////////////////////////////////////////////////////////////////////////////////
void ProcessPreRenderShaders( GLWindow*, void* ) 
{
    g_dPose = g_dPose* GLCart2T( g_fSpeed,0,0,0,0,g_fTurnrate );
    cam.SetPose( g_dPose );

    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glClearColor( 0.0, 0.0, 0.0, 1 );

    //_SetupLighting();

    cam.RenderToTexture(); // will render to texture, then copy texture to CPU memory
}

/////////////////////////////////////////////////////////////////////////////////////////
void ShowCameraAndTextures( GLWindow*, void* )
{
    // copy range data into point cloud
    if( bOrtho ){
        cam.SetOrtho( fOrthoSize, fOrthoSize );
    }
    else{
        cam.UnSetOrtho();
    }
    cam.DepthTo3D( PointCloud.RangeDataRef() ); 
    PointCloud.SetPose( cam.GetPoseRef() );
    PointCloud.draw();

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
        GLTeapot()
        {
            SetName("Teapot"); 
            m_nSelectionId = -1;
        }

        void MouseOver( int nId )
        {
            printf("mouse over %d\n", nId );
        }

        void select( int nId )
        {
            printf("select %d\n", nId );
        }


        void draw()
        {
            if( m_nSelectionId == -1 ){
                m_nSelectionId = AllocSelectionId();
            }

//            glPushName( m_nSelectionId );
            glFrontFace(GL_CW);
            glutSolidTeapot( 8 );
            glFrontFace(GL_CCW);
//            glPopName();
        }

        GLint m_nSelectionId;
};

/*
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
 */

/////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char** argv )
{
    GetPot cl( argc, argv );

    std::string sMesh = cl.follow( "Terrain.ac", 1, "-mesh" );

    // init window
    GuiWindow* pWin = new GuiWindow( 0, 0, 1024, 768, "Simple Gui Demo" );

    // load mesh
    const struct aiScene* pScene;
    pScene = aiImportFile( sMesh.c_str(), aiProcess_Triangulate | aiProcess_GenNormals );

    GLGrid grid;
    grid.SetPerceptable( false );

    GLMesh mesh;
    mesh.Init( pScene );

    GLTeapot teapot;

    PeaksHeightMap p;
    GLHeightMap hm(&p);

    // register objects
    pWin->AddChildToRoot( &hm );
    //    pWin->AddChildToRoot( &mesh );
    pWin->AddChildToRoot( &teapot );
    pWin->AddChildToRoot( &grid );

    int w = 128;
    int h = 128;
    Eigen::Matrix3d dK;   // computer vision K matrix
    dK << w,0,w/2,0,h,h/2,0,0,1;

    cam.Init( &pWin->SceneGraph(), g_dPose, dK, w,h, eSimCamRGB | eSimCamDepth | eSimCamNormals );

    glEnable( GL_LIGHT0 );    // activate light0
    glEnable( GL_LIGHTING );  // enable lighting

    pWin->LookAt( -70, -70, -50, 0,0,0, 0,0,-1 );

    //    _SetupLighting();

    // add our callbacks
    pWin->AddPreRenderCallback( ProcessPreRenderShaders, NULL );
    pWin->AddPostRenderCallback( ShowCameraAndTextures, NULL );

    return( pWin->Run() );
}

