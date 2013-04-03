#include <iostream>
#include <boost/bind.hpp>
#include <Eigen/Eigen>

#include <pangolin/pangolin.h>
#include <SceneGraph/SceneGraph.h>


#include "CVarHelpers.h"
#include <CVars/CVar.h>
#include "BulletWrapper.h"

using namespace std;
using namespace CVarUtils;

/*

TODO: load a car
TODO: GLDebugDrawer  -- whatever that is
TODO: mesh collision
TODO: load quake map

*/


struct App
{
    ///////////////////////////////////////////////////////////////////
    Sim*                        m_pSim;

    SceneGraph::GLShadowLight   m_light;
    SceneGraph::GLBox           m_ground;
    SceneGraph::GLCube          m_cube;
    SceneGraph::GLBox           m_ramp1;
    SceneGraph::GLBox           m_ramp2;
    SceneGraph::GLGrid          m_grid;
    SceneGraph::GLSceneGraph&   m_rSceneGraph;

    SceneGraph::GLCylinder      m_Wheel;

    Eigen::Vector6d             m_dRamp1Pose;
    Eigen::Vector3d             m_dRamp1Extent;

    Eigen::Vector6d             m_dRamp2Pose;
    Eigen::Vector3d             m_dRamp2Extent;

    double                      m_dGravity;

    ///////////////////////////////////////////////////////////////////
    App( SceneGraph::GLSceneGraph& glGraph) : m_rSceneGraph( glGraph )
    {
        m_pSim = NULL;

        m_dRamp1Pose << -2, 0, -15, 0, -M_PI/11, 0;
        m_dRamp1Extent << 10, 4, 1;
        m_dGravity = 9.8;

        // Allow user to change these values from the console.
        AttachCVar( "sim.ramp1.Pose", &m_dRamp1Pose, "Use this to change the pose of the first ramp" );
        AttachCVar( "sim.ramp1.Extent", &m_dRamp1Extent, "Use this to change the extent of the first ramp" );
        AttachCVar( "sim.ramp2.Pose", &m_dRamp2Pose, "Use this to change the pose of the first ramp" );
        AttachCVar( "sim.ramp2.Extent", &m_dRamp2Extent, "Use this to change the extent of the first ramp" );
        AttachCVar( "sim.Gravity", &m_dGravity, "Use this to change gravity" );

        InitReset(); // this will populate the scene graph with objects and
                     // register these objects with the simulator.
    }

    ///////////////////////////////////////////////////////////////////
    /// Re-allocate the simulator each time
    void InitReset()
    {
        m_rSceneGraph.Clear();

        m_light.SetPosition( 10,10,-100 );
        m_rSceneGraph.AddChild( &m_light );

        m_grid.SetNumLines(20);
        m_grid.SetLineSpacing(1);
        m_rSceneGraph.AddChild(&m_grid);

        Eigen::Vector6d dCubePose;
        dCubePose << 0, 0, -20, M_PI_4, M_PI_4, 0.1;
        m_cube.SetPose( dCubePose );
        m_rSceneGraph.AddChild( &m_cube );

        m_ground.SetPose( 0,0,0,0,0,0 );
        m_ground.SetExtent( 20,20,1 );
        m_rSceneGraph.AddChild( &m_ground );

        m_ramp1.SetPose( m_dRamp1Pose );
        m_ramp1.SetExtent( m_dRamp1Extent );
        m_rSceneGraph.AddChild( &m_ramp1 );

        m_ramp2.SetPose( 2, 0, -10, 0, M_PI/10, M_PI/8);
        m_ramp2.SetExtent( 10, 4, 1 );
        m_rSceneGraph.AddChild( &m_ramp2 );

        m_light.AddShadowReceiver( &m_ground );
        m_light.AddShadowCasterAndReceiver( &m_cube );
        m_light.AddShadowCasterAndReceiver( &m_ramp1 );
        m_light.AddShadowCasterAndReceiver( &m_ramp2 );

        m_Wheel.Init( 1, 1, 1, 32, 10 );
        m_Wheel.SetPose( 0, 0, -5, M_PI/2, 0, 0 );
        m_rSceneGraph.AddChild( &m_Wheel );

        m_light.AddShadowCasterAndReceiver( &m_Wheel );

        // Send our GLObjects to the simulator.
        // NB: Sim keeps a pointer to our objects, so we can't delete them
        // NB: Sim also changes our objects position (that's why we register them)
        if( m_pSim ){
            delete m_pSim;
        }
        m_pSim = new Sim;
        m_pSim->Init(  m_dGravity );
        m_pSim->RegisterObject( &m_ground, 0 /* no mass ==> static object */ );
        m_pSim->RegisterObject( &m_ramp1, 0 /* no mass ==> static object */ );
        m_pSim->RegisterObject( &m_ramp2, 0 /* no mass==> static object */ );
        m_pSim->RegisterObject( &m_cube, 1 );
    }

};


///////////////////////////////////////////////////////////////////
int main( int /*argc*/, char** /*argv*/ )
{
    // Create OpenGL window in single line thanks to GLUT
    pangolin::CreateGlutWindowAndBind("Main",640,480);
    SceneGraph::GLSceneGraph::ApplyPreferredGlSettings();
    glewInit();

    // sinle application context holds everything
    SceneGraph::GLSceneGraph  glGraph;
    App app( glGraph );

    //////////////////////////////////////////////
    // <Pangolin boilerplate>
    const SceneGraph::AxisAlignedBoundingBox bbox = glGraph.ObjectAndChildrenBounds();
    const Eigen::Vector3d center = bbox.Center();
    const double size = bbox.Size().norm();
    const double far = 10*size;
    const double near = far / 1E3;

    // Define Camera Render Object (for view / scene browsing)
    pangolin::OpenGlRenderState stacks3d(
            pangolin::ProjectionMatrix(640,480,420,420,320,240,near,far),
            pangolin::ModelViewLookAt(center(0), center(1) + size, center(2) - size/4, 
                center(0), center(1), center(2), pangolin::AxisNegZ) );

    // We define a new view which will reside within the container.
    pangolin::View view3d;

    // We set the views location on screen and add a handler which will
    // let user input update the model_view matrix (stacks3d) and feed through
    // to our scenegraph
    view3d.SetBounds(0.0, 1.0, 0.0, 1.0, -640.0f/480.0f);
    view3d.SetHandler( new SceneGraph::HandlerSceneGraph( glGraph, stacks3d) );
    view3d.SetDrawFunction( SceneGraph::ActivateDrawFunctor( glGraph, stacks3d) );

    // Add our views as children to the base container.
    pangolin::DisplayBase().AddDisplay(view3d);

    // register a keyboard hook to trigger the reset method
    pangolin::RegisterKeyPressCallback( pangolin::PANGO_CTRL + 'r', 
            boost::bind( &App::InitReset, &app ) );

    // </Pangolin boilerplate>

    // Default hooks for exiting (Esc) and fullscreen (tab).
    while( !pangolin::ShouldQuit() ) {
        // Clear whole screen
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        // Swap frames and Process Events
        pangolin::FinishGlutFrame();

        // step the simulator
        app.m_pSim->StepSimulation();
    }

    return 0;
}
