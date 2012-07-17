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

    GLSceneGraph glGraph;
    glGraph.ApplyPreferredGlSettings();

    // Define objects to draw
    GLGrid glGrid(50,2.0, true);
    GLWayPoint glWaypoint;
    GLLineStrip glLineStrip;

    glLineStrip.SetPoint(1,1,1);
    glLineStrip.SetPoint(2,1,1);
    glLineStrip.SetPoint(3,1,1);
    glLineStrip.SetPoint(3,2,1);
    glLineStrip.SetPoint(3,3,1);

    // Add objects to scenegraph
    glGraph.AddChild(&glGrid);
    glGraph.AddChild(&glWaypoint);
    glGraph.AddChild(&glLineStrip);

    // Define Camera Render Object (for view / scene browsing)
    pangolin::OpenGlRenderState renderState(
                ProjectionMatrix(640,480,420,420,320,240,0.1,1000),
                Pose(0,0,-3, AxisNegZ, AxisY)
                );

    // Add viewport to window and provide 3D Handler
    View& v3d = pangolin::CreateDisplay()
            .SetBounds(0.0, 1.0, 0.0, 1.0, -640.0f/480.0f)
            .SetHandler(new HandlerSceneGraph(glGraph,renderState,AxisZ))
            .SetDrawFunction(ActivateScissorClearDrawFunctor(glGraph, renderState));

    // Default hooks for exiting (Esc) and fullscreen (tab).
    while( !pangolin::ShouldQuit() )
    {
        v3d.ActivateScissorAndClear(renderState);

        // Swap frames and Process Events
        FinishGlutFrame();

        usleep(1000);
    }

    return 0;
}
