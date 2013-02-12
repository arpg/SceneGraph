
#include <pangolin/pangolin.h>
#include <SceneGraph/SceneGraph.h>
#include <SceneGraph/SimCam.h>
#include <boost/bind.hpp>
#include <Mvlpp/Mvl.h>
#include <CVars/CVar.h>

#include "CVarHelpers.h"

using namespace std;

namespace sg =SceneGraph;
namespace pango =pangolin;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char USAGE[] =
    "Usage:     SimCamDEMO -mesh <name> -lcmod <file> -rcmod <file>\n" "\n";


/**************************************************************************************************
 *
 * VARIABLES
 *
 **************************************************************************************************/

// Global CVars
bool&            g_bShowFrustum = CVarUtils::CreateCVar( "Cam.ShowFrustum", true, "Show cameras viewing frustum." );
Eigen::Vector6d& g_vCamPose     = CVarUtils::CreateCVar( "Cam.Pose", Eigen::Vector6d( Eigen::Vector6d::Zero() ),
                                  "Camera's pose. Left is dominant camera." );


// Cameras
sg::GLSimCam glCamLeft;     // reference camera we move
sg::GLSimCam glCamRight;    // reference camera we move

// Camera Extrinsics
Eigen::Matrix4d g_mTvl;
Eigen::Matrix4d g_mTvr;

// Reference Camera Controls
Eigen::Vector6d g_vCamVel = Eigen::Vector6d::Zero();

// Global Variables
Eigen::Vector6d g_dBaseline;
unsigned int    g_nImgWidth;
unsigned int    g_nImgHeight;

// //////////////////////////////////////////////////////////////////////////////
void _ResetCamera()
{
    // set camera from initial view
    g_vCamPose.setZero();
}

// //////////////////////////////////////////////////////////////////////////////
void _MoveCamera(
        unsigned int DF,
        float        X
        )
{
    g_vCamVel( DF ) += X;
}

// //////////////////////////////////////////////////////////////////////////////
void _StopCamera()
{
    g_vCamVel.setZero();
}

// //////////////////////////////////////////////////////////////////////////////
Eigen::Vector6d RightCamPose()
{
    Eigen::Matrix4d Tlr;
    Tlr = mvl::TInv(g_mTvl) * g_mTvr;
    Eigen::Matrix4d T;
    // T = Twl
    T = mvl::Cart2T( g_vCamPose );
    // Twr = Twl * Tlr
    T = T * Tlr;
    return mvl::T2Cart(T);
}

// //////////////////////////////////////////////////////////////////////////////
inline void UpdateCameras()
{
    Eigen::Matrix4d T;

    // move camera by user's input
    T          = mvl::Cart2T( g_vCamPose );
    T          = T * mvl::Cart2T( g_vCamVel );
    g_vCamPose = mvl::T2Cart( T );

    glCamLeft.SetPoseRobot( mvl::Cart2T( g_vCamPose ) );
    glCamRight.SetPoseRobot( mvl::Cart2T( RightCamPose() ) );

    glCamLeft.RenderToTexture();    // will render to texture, then copy texture to CPU memory
    glCamRight.RenderToTexture();    // will render to texture, then copy texture to CPU memory
}

/**************************************************************************************************
 *
 * MAIN
 *
 **************************************************************************************************/
int main(
        int    argc,
        char** argv
        )
{
    if( argc < 1 ) {
        cout << USAGE << endl;

        exit( 0 );
    }

    // parse arguments
    GetPot cl( argc, argv );

    if( cl.search( 3, "--help", "-help", "-h" ) ) {
        cout << USAGE << endl;

        exit( 0 );
    }


    string sMesh             = cl.follow( "CityBlock.obj", 1, "-mesh" );
    string sLeftCameraModel  = cl.follow( "lcmod.xml", 1, "-lcmod" );
    string sRightCameraModel = cl.follow( "rcmod.xml", 1, "-rcmod" );

    // get camera model files
    mvl::CameraModel LeftCamModel( sLeftCameraModel );
    mvl::CameraModel RightCamModel( sRightCameraModel );

    // get some camera parameters
    Eigen::Matrix3d K = LeftCamModel.K();

    g_nImgWidth  = LeftCamModel.Width();
    g_nImgHeight = LeftCamModel.Height();

    // for the baseline we assume the left is the dominant camera
    g_mTvl = LeftCamModel.GetPose();
    g_mTvr = RightCamModel.GetPose();

    // Create OpenGL window in single line thanks to GLUT
    pango::CreateGlutWindowAndBind( "SimCamDEMO", 1280, 640 );
    sg::GLSceneGraph::ApplyPreferredGlSettings();

    // Scenegraph to hold GLObjects and relative transformations
    sg::GLSceneGraph glGraph;

    // set up mesh
    sg::GLMesh glMesh;

    try
    {
        glMesh.Init( sMesh );
        glMesh.SetPerceptable( true );
        glGraph.AddChild( &glMesh );

        cout << "MeshLogger: Mesh '" << sMesh << "' loaded." << endl;
    }
    catch( exception e )
    {
        cerr << "SimCamDEMO: Cannot load mesh. Check file exists." << endl;

        exit( 0 );
    }

    // define grid object
    sg::GLGrid glGrid;
    glGrid.SetPose( 0, 0, 1, 0, 0, 0 );
    glGraph.AddChild( &glGrid );


    // initialize cameras
    glCamLeft.Init( &glGraph, mvl::Cart2T( g_vCamPose ), K, g_nImgWidth, g_nImgHeight,
                        sg::eSimCamLuminance | SceneGraph::eSimCamDepth );

    glCamRight.Init( &glGraph, mvl::Cart2T( RightCamPose() ), K, g_nImgWidth, g_nImgHeight,
                     sg::eSimCamLuminance );

    // Define Camera Render Object (for view / scene browsing)
    pango::OpenGlRenderState glState( pango::ProjectionMatrix( 640, 480, 420, 420, 320, 240, 0.1, 1000 ),
                                      pango::ModelViewLookAt( -6, 0, -30, 1, 0, 0, pango::AxisNegZ ) );

    // Pangolin abstracts the OpenGL viewport as a View.
    // Here we get a reference to the default 'base' view.
    pango::View& glBaseView = pango::DisplayBase();

    // We define a new view which will reside within the container.
    pango::View glView3D;

    // We set the views location on screen and add a handler which will
    // let user input update the model_view matrix (stacks3d) and feed through
    // to our scenegraph
    glView3D.SetBounds( 0.0, 1.0, 0.0, 3.0 / 4.0, 640.0f / 480.0f );
    glView3D.SetHandler( new sg::HandlerSceneGraph( glGraph, glState, pango::AxisNegZ ) );
    glView3D.SetDrawFunction( sg::ActivateDrawFunctor( glGraph, glState ) );

    // display images
    sg::ImageView glLeftImg( true, true );
    glLeftImg.SetBounds( 0.5, 1.0, 3.0 / 4.0, 1.0, (double)g_nImgWidth / g_nImgHeight );

    sg::ImageView glRightImg( true, true );
    glRightImg.SetBounds( 0.0, 0.5, 3.0 / 4.0, 1.0, (double)g_nImgWidth / g_nImgHeight );

    // Add our views as children to the base container.
    glBaseView.AddDisplay( glView3D );
    glBaseView.AddDisplay( glLeftImg );
    glBaseView.AddDisplay( glRightImg );

    // register key callbacks
    pango::RegisterKeyPressCallback( 'e', boost::bind( _MoveCamera, 0, 0.01 ) );
    pango::RegisterKeyPressCallback( 'q', boost::bind( _MoveCamera, 0, -0.01 ) );
    pango::RegisterKeyPressCallback( 'a', boost::bind( _MoveCamera, 1, -0.01 ) );
    pango::RegisterKeyPressCallback( 'd', boost::bind( _MoveCamera, 1, 0.01 ) );
    pango::RegisterKeyPressCallback( 'w', boost::bind( _MoveCamera, 2, -0.01 ) );
    pango::RegisterKeyPressCallback( 's', boost::bind( _MoveCamera, 2, 0.01 ) );
    pango::RegisterKeyPressCallback( 'u', boost::bind( _MoveCamera, 3, -0.005 ) );
    pango::RegisterKeyPressCallback( 'o', boost::bind( _MoveCamera, 3, 0.005 ) );
    pango::RegisterKeyPressCallback( 'i', boost::bind( _MoveCamera, 4, 0.005 ) );
    pango::RegisterKeyPressCallback( 'k', boost::bind( _MoveCamera, 4, -0.005 ) );
    pango::RegisterKeyPressCallback( 'j', boost::bind( _MoveCamera, 5, -0.005 ) );
    pango::RegisterKeyPressCallback( 'l', boost::bind( _MoveCamera, 5, 0.005 ) );
    pango::RegisterKeyPressCallback( ' ', boost::bind( _StopCamera ) );
    pango::RegisterKeyPressCallback( pango::PANGO_CTRL + 'r', boost::bind( _ResetCamera ) );

    // temporal buffer to capture images
    char* pBuffImg   = (char*)malloc( g_nImgWidth * g_nImgHeight );

    // Default hooks for exiting (Esc) and fullscreen (tab).
    while( !pango::ShouldQuit() ) {
        // Clear whole screen
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        // pre-render stuff
        UpdateCameras();

        // render cameras
        glView3D.Activate( glState );

        if( g_bShowFrustum ) {
            // show the camera
            glCamLeft.DrawCamera();
            glCamRight.DrawCamera();
        }

        // show left images
        if( glCamLeft.CaptureGrey( pBuffImg ) ) {
            glLeftImg.SetImage( pBuffImg, g_nImgWidth, g_nImgHeight, GL_INTENSITY, GL_LUMINANCE, GL_UNSIGNED_BYTE );
        }

        // show right image
        if( glCamRight.CaptureGrey( pBuffImg ) ) {
            glRightImg.SetImage( pBuffImg, g_nImgWidth, g_nImgHeight, GL_INTENSITY, GL_LUMINANCE, GL_UNSIGNED_BYTE );
        }

        // Swap frames and Process Events
        pango::FinishGlutFrame();

        // Pause for 1/60th of a second.
        usleep( 1E6 / 60 );
    }

    return 0;
}
