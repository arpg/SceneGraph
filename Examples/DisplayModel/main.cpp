#include <iostream>
#include <pangolin/pangolin.h>
#include <SceneGraph/SceneGraph.h>
#include <Eigen/Eigen>


using namespace SceneGraph;
using namespace pangolin;
using namespace std;

int main( int /*argc*/, char** /*argv[]*/ )
{  
    // Create OpenGL window in single line thanks to GLUT
    pangolin::CreateGlutWindowAndBind("Main",640,480);
    GLSceneGraph::ApplyPreferredGlSettings();

    GLSceneGraph glGraph;
    GLSceneGraph glGraph2d;

    // Define objects to draw
    GLGrid glGrid(50,2.0, true);

    GLAxis glAxis;
    glAxis.SetPose(-1,-2,-0.1, 0, 0, M_PI/4);
    glAxis.SetScale(0.25);

    GLWayPoint glWaypoint;
    glWaypoint.SetPose(0.5,0.5,-0.1,0,0,0);

    GLLineStrip glLineStrip;
    for(double t=0; t < 10*M_PI; t+= M_PI/10) {
        glLineStrip.SetPoint(cos(t)+2, sin(t)+2, -0.2*t);
    }

    GLText glText3d("3D Floating Text", -1, 1, -1);

    // Add objects to scenegraph
    glGraph.AddChild(&glGrid);
    glGraph.AddChild(&glWaypoint);
    glGraph.AddChild(&glLineStrip);
    glGraph.AddChild(&glAxis);
    glGraph.AddChild(&glText3d);

    GLText glText2d("2D Overlay Text", 10, 460);
    glGraph2d.AddChild(&glText2d);

    // Define Camera Render Object (for view / scene browsing)
    pangolin::OpenGlRenderState stacks3d(
                ProjectionMatrix(640,480,420,420,320,240,0.1,1000),
                ModelViewLookAt(0,-2,-4, 0,1,0, AxisNegZ)
                );

    // Use different orthographic render state for 2D drawing.
    // Virtualise screen resolution to 640x480
    pangolin::OpenGlRenderState stacks2d(
                ProjectionMatrixOrthographic(480,0,0,640,0,1000)
                );

    // Add viewport to window and provide 3D Handler
    View& view = pangolin::CreateDisplay()
            .SetBounds(0.0, 1.0, 0.0, 1.0, -640.0f/480.0f)
            .SetHandler(new HandlerSceneGraph(glGraph,stacks3d,AxisNegZ))
            .SetDrawFunction(ActivateScissorClearDrawFunctor3d2d(glGraph, stacks3d, glGraph2d, stacks2d));

    // Default hooks for exiting (Esc) and fullscreen (tab).
    while( !pangolin::ShouldQuit() )
    {
        view.ActivateScissorAndClear(stacks3d);

        // Swap frames and Process Events
        FinishGlutFrame();

        usleep(1E6 / 60);
    }

    return 0;
}
