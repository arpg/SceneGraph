#include <iostream>
#include <boost/bind.hpp>
#include <Eigen/Eigen>

#include <pangolin/pangolin.h>
#include <SceneGraph/SceneGraph.h>

using namespace std;

void GlobalKeyHook(std::string str)
{
    cout << str << endl;
}

int main( int /*argc*/, char** /*argv[]*/ )
{
    // Create OpenGL window in single line thanks to GLUT
    pangolin::CreateWindowAndBind("Main",640*2,480);
    SceneGraph::GLSceneGraph::ApplyPreferredGlSettings();
    
    // Disable line smooth so that we can have transparent objects without
    // far-to-near ordered rendering.
    glDisable(GL_LINE_SMOOTH);    

    // Scenegraph to hold GLObjects and relative transformations
    SceneGraph::GLSceneGraph glGraph;

    SceneGraph::GLLight light(10,10,-100);
    glGraph.AddChild(&light);

    // Define axis object, and set its pose
    SceneGraph::GLAxis glAxis;
    glAxis.SetPose( 0, 0, 0, 0, 0, 0);
    glAxis.SetScale(0.25);
    glGraph.AddChild(&glAxis);

#ifndef HAVE_GLES
    // Define movable waypoint object with velocity
    SceneGraph::GLWayPoint glWaypoint;
    glWaypoint.SetPose(0.5,0.5,-0.1,0,0,0);
    glGraph.AddChild(&glWaypoint);

    // Optionally clamp waypoint to specific plane
    glWaypoint.ClampToPlane(Eigen::Vector4d(0,0,1,0));
#endif


    // Define 3D spiral using a GLCachedPrimitives object
    SceneGraph::GLCachedPrimitives glSpiral(GL_LINE_STRIP, SceneGraph::GLColor(1.0f,0.7f,0.2f));
    for(double t=0; t < 10*M_PI; t+= M_PI/50) {
        glSpiral.AddVertex(Eigen::Vector3d(cos(t)+2, sin(t)+2, -0.1*t) );
    }
    glGraph.AddChild(&glSpiral);

    // Define grid object -- this will be alpha transparent;
    // TODO: Always render near to far.
    SceneGraph::GLGrid glGrid( 50, 2.0, true );
    glGrid.SetColors(
                SceneGraph::GLColor(1.0f, 1.0f, 1.0f, 0.3f),
                SceneGraph::GLColor(0.5f, 0.5f, 0.5f, 1.0f)
                );
    glGraph.AddChild(&glGrid);

    // Define Camera Render Object (for view / scene browsing)
    pangolin::OpenGlRenderState stacks3d(
        pangolin::ProjectionMatrix(640,480,420,420,320,240,0.1,1000),
        pangolin::ModelViewLookAt(0,-2,-4, 0,1,0, pangolin::AxisNegZ)
    );

    // Define second camera render object
    pangolin::OpenGlRenderState stacks3d_2(
        pangolin::ProjectionMatrix(640,480,420,420,320,240,0.1,1000),
        pangolin::ModelViewLookAt(2,-1,-2, 0,1,0, pangolin::AxisNegZ)
    );

    // Pangolin abstracts the OpenGL viewport as a View.
    // Here we get a reference to the default 'base' view.
    pangolin::View& container = pangolin::DisplayBase();

    // We define a new view which will reside within the container.
    pangolin::View view3d;

    // We set the views location on screen and add a handler which will
    // let user input update the model_view matrix (stacks3d) and feed through
    // to our scenegraph
    view3d.SetBounds(0.0, 1.0, 0.0, 1.0/2.0, 640.0f/480.0f)
          .SetHandler(new SceneGraph::HandlerSceneGraph(glGraph,stacks3d,pangolin::AxisNegZ))
          .SetDrawFunction(SceneGraph::ActivateDrawFunctor(glGraph, stacks3d));

    // We can define another view on the same scenegraph. We can also
    // render a second scenegraph as a 2D overlay using the different
    // 2D ModelView and Projection matrices (stacks3d)
    pangolin::View view3d_2d;
    view3d_2d.SetBounds(0.0,1.0, 1.0/2.0, 1.0, 640.0f/480.0f)
          .SetHandler(new SceneGraph::HandlerSceneGraph(glGraph,stacks3d_2,pangolin::AxisNegZ))
          .SetDrawFunction(SceneGraph::ActivateDrawFunctor(glGraph, stacks3d_2));

    // Add our views as children to the base container.
    container.AddDisplay(view3d);
    container.AddDisplay(view3d_2d);

    // Demonstration of how we can register a keyboard hook to trigger a method
    pangolin::RegisterKeyPressCallback( pangolin::PANGO_CTRL + 'r', boost::bind(GlobalKeyHook, "You Pushed ctrl-r!" ) );

    // Add keyhook to save window contents (including alpha). The framebuffer is saved just before it is swapped
    pangolin::RegisterKeyPressCallback( 's', boost::bind(&pangolin::View::SaveOnRender, &pangolin::DisplayBase(), "window_OnRender" ) );

    // Add keyhook to save a particular view (including alpha) at 4 times the resolution of the screen. This creates an FBO and renders into it straight away.
    pangolin::RegisterKeyPressCallback( 'r', boost::bind(&pangolin::View::SaveRenderNow, &view3d, "view3d_RenderNow", 4 ) );

    glClearColor( 0, 0, 0, 0 );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );


    // Default hooks for exiting (Esc) and fullscreen (tab).
    while( !pangolin::ShouldQuit() )
    {
        // Clear whole screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Swap frames and Process Events
        pangolin::FinishFrame();

        // Pause for 1/60th of a second.
        usleep(1E6 / 60);
    }

    return 0;
}
