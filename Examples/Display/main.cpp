#include <iostream>
#include <chrono>
#include <thread>
#include <Eigen/Eigen>

#include <pangolin/pangolin.h>
#include <SceneGraph/SceneGraph.h>

using namespace std;

int main( int /*argc*/, char** /*argv[]*/ )
{
    // Create OpenGL window in single line thanks to GLUT
    pangolin::CreateWindowAndBind("Main",640,480);
    SceneGraph::GLSceneGraph::ApplyPreferredGlSettings();

    // Scenegraph to hold GLObjects and relative transformations
    SceneGraph::GLSceneGraph glGraph;

    // Define grid object
    SceneGraph::GLGrid glGrid( 50, 2.0, true );
    glGraph.AddChild(&glGrid);    

    // Define axis object, and set its pose
    SceneGraph::GLAxis glAxis;
    glAxis.SetPose( 0, 0, 0, 0, 0, 0);
    glAxis.SetScale(0.25);
    glGraph.AddChild(&glAxis);
    
    // Define 3D spiral using a GLCachedPrimitives object
    SceneGraph::GLCachedPrimitives glSpiral;//(GL_LINE_STRIP, SceneGraph::GLColor(1.0f,0.7f,0.2f) );
    for(double t=0; t < 10*M_PI; t+= M_PI/50) {
        glSpiral.AddVertex(Eigen::Vector3d(cos(t), sin(t), -0.1*t) );
    }
//    glSpiral.SetColor(0.5,0.8,0.6);
    glGraph.AddChild(&glSpiral);

    // Define 3D floating text object
    SceneGraph::GLText glText3d("3D Floating Text", -1, 1, -1);
    glGraph.AddChild(&glText3d);    

#ifndef HAVE_GLES
    SceneGraph::GLMovableAxis glMovableAxis;
    glMovableAxis.SetPosition(-3,3,-1);
    glGraph.AddChild(&glMovableAxis);

    SceneGraph::GLAxisAlignedBox glBox;
    glBox.SetResizable();
    glMovableAxis.AddChild(&glBox);

    // Define movable waypoint object with velocity
    SceneGraph::GLWayPoint glWaypoint;
    glWaypoint.SetPose(0.5,0.5,0.5,0,0,0);
    glGraph.AddChild(&glWaypoint);
    glWaypoint.SetColor(0.8,0.5,1);
    glWaypoint.SetLocked(false);

    // Optionally clamp waypoint to specific plane
//    glWaypoint.ClampToPlane(Eigen::Vector4d(0,0,1,0));
#endif

    // Define Camera Render Object (for view / scene browsing)
    pangolin::OpenGlRenderState stacks3d(
        pangolin::ProjectionMatrix(640,480,420,420,320,240,0.1,1000),
        pangolin::ModelViewLookAt(0,-2,-4, 0,1,0, pangolin::AxisNegZ)
    );

    // We define a new view which will reside within the container.
    pangolin::View view3d;

    // We set the views location on screen and add a handler which will
    // let user input update the model_view matrix (stacks3d) and feed through
    // to our scenegraph
    view3d.SetBounds(0.0, 1.0, 0.0, 1.0, 640.0f/480.0f)
          .SetHandler(new SceneGraph::HandlerSceneGraph(glGraph,stacks3d,pangolin::AxisNegZ))
          .SetDrawFunction(SceneGraph::ActivateDrawFunctor(glGraph, stacks3d));

    // Add our views as children to the base container.
    pangolin::DisplayBase().AddDisplay(view3d);

    // Default hooks for exiting (Esc) and fullscreen (tab).
    while( !pangolin::ShouldQuit() )
    {
        // Clear whole screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Swap frames and Process Events
        pangolin::FinishFrame();

        // Pause for 1/60th of a second.
        std::this_thread::sleep_for(std::chrono::milliseconds(1000/60));
    }

    return 0;
}
