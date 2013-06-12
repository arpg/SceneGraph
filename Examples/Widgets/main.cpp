#include <iostream>
#include <boost/bind.hpp>
#include <Eigen/Eigen>

#include <pangolin/pangolin.h>
#include <SceneGraph/SceneGraph.h>
#include "Widgets/GLWidgetView.h"

#define WINDOW_HEIGHT 480.0f
#define WINDOW_WIDTH 640.0f

#define UI_PANEL_HEIGHT WINDOW_HEIGHT
#define UI_PANEL_WIDTH 200

using namespace std;

void Usage() {
    cout << "Usage: ModelViewer filename" << endl;
}

bool g_bCheckbox1 = false;
bool g_bCheckbox2 = true;

void WidgetDrawFunction (nv::GlutUIContext& context,nv::Rect& rect)
{
    context.beginFrame(nv::GroupFlags_GrowDownFromLeft,rect);
        context.doCheckButton(nv::Rect(),"Checkbox1:", &g_bCheckbox1);
        context.doCheckButton(nv::Rect(),"Checkbox2:", &g_bCheckbox2);
        context.beginGroup(nv::GroupFlags_GrowRightFromTop);
        context.doLabel(nv::Rect(),"123");
            context.doLabel(nv::Rect(),"456");
         context.endGroup();
    context.endFrame();
}

void GlobalKeyHook(std::string str)
{
    cout << str << endl;
}

int main( int argc, char* argv[] )
{
    // Create OpenGL window in single line thanks to GLUT
    pangolin::CreateWindowAndBind("Main",640,480);
    SceneGraph::GLSceneGraph::ApplyPreferredGlSettings();
    glewInit();

    // Scenegraph to hold GLObjects and relative transformations
    SceneGraph::GLSceneGraph glGraph;

    SceneGraph::GLLight light(10,10,-100);
    glGraph.AddChild(&light);

    // Define grid object
    SceneGraph::GLGrid glGrid(50,2.0, true);

    // Define axis object, and set its pose
    SceneGraph::GLAxis glAxis;
    glAxis.SetPose(-1,-2,-0.1, 0, 0, M_PI/4);
    glAxis.SetScale(0.25);

    SceneGraph::GLMovableAxis glMovableAxis;
    glMovableAxis.SetPosition(-3,3,-1);

    SceneGraph::GLAxisAlignedBox glBox;
    glBox.SetResizable();

    // Define movable waypoint object with velocity
    SceneGraph::GLWayPoint glWaypoint;
    glWaypoint.SetPose(0.5,0.5,-0.1,0,0,0);

    // Optionally clamp waypoint to specific plane
    glWaypoint.ClampToPlane(Eigen::Vector4d(0,0,1,0));

    // Define 3D spiral using a GLCachedPrimitives object
    SceneGraph::GLCachedPrimitives glSpiral(GL_LINE_STRIP, SceneGraph::GLColor(1.0f,0.7f,0.2f));
    for(double t=0; t < 10*M_PI; t+= M_PI/50) {
        glSpiral.AddVertex(Eigen::Vector3d(cos(t)+2, sin(t)+2, -0.1*t) );
    }

    // Define 3D floating text object
    SceneGraph::GLText glText3d("3D Floating Text", -1, 1, -1);


    // Add objects to scenegraph
    glGraph.AddChild(&glGrid);
    glGraph.AddChild(&glWaypoint);
    glGraph.AddChild(&glSpiral);
    glGraph.AddChild(&glAxis);
    glGraph.AddChild(&glText3d);
    glGraph.AddChild(&glMovableAxis);
    glMovableAxis.AddChild(&glBox);


    // Define Camera Render Object (for view / scene browsing)
    pangolin::OpenGlRenderState stacks3d(
        pangolin::ProjectionMatrix(640,480,420,420,320,240,0.1,1000),
        pangolin::ModelViewLookAt(0,-2,-4, 0,1,0, pangolin::AxisNegZ)
    );

    // Pangolin abstracts the OpenGL viewport as a View.
    // Here we get a reference to the default 'base' view.
    pangolin::View& container = pangolin::DisplayBase();

    // We define a new view which will reside within the container.
    pangolin::View view3d;

    // We set the views location on screen and add a handler which will
    // let user input update the model_view matrix (stacks3d) and feed through
    // to our scenegraph
    view3d.SetBounds(0.0, 1.0, 0.0, 1.0, 640.0f/480.0f)
          .SetHandler(new SceneGraph::HandlerSceneGraph(glGraph,stacks3d,pangolin::AxisNegZ))
          .SetDrawFunction(SceneGraph::ActivateDrawFunctor(glGraph, stacks3d));

    // Add our views as children to the base container.
    container.AddDisplay(view3d);


    GLWidgetView panel;
    panel.Init(0,1,0,Attach::Pix(200),&WidgetDrawFunction);

    container.AddDisplay(panel);

    // Demonstration of how we can register a keyboard hook to trigger a method
    pangolin::RegisterKeyPressCallback( pangolin::PANGO_CTRL + 'r', boost::bind(GlobalKeyHook, "You Pushed ctrl-r!" ) );

    // Add keyhook to save window contents (including alpha). The framebuffer is saved just before it is swapped
    pangolin::RegisterKeyPressCallback( 's', boost::bind(&pangolin::View::SaveOnRender, &pangolin::DisplayBase(), "window_OnRender" ) );

    // Add keyhook to save a particular view (including alpha) at 4 times the resolution of the screen. This creates an FBO and renders into it straight away.
    pangolin::RegisterKeyPressCallback( 'r', boost::bind(&pangolin::View::SaveRenderNow, &view3d, "view3d_RenderNow", 4 ) );

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
