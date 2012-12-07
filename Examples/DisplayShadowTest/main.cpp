#include <iostream>
#include <boost/bind.hpp>
#include <Eigen/Eigen>

#include <pangolin/pangolin.h>
#include <SceneGraph/SceneGraph.h>

using namespace std;

int main( int /*argc*/, char** /*argv[]*/ )
{
    // Create OpenGL window in single line thanks to GLUT
    pangolin::CreateGlutWindowAndBind("Main",640,480);
    SceneGraph::GLSceneGraph::ApplyPreferredGlSettings();
    glewInit();

    // Scenegraph to hold GLObjects and relative transformations
    SceneGraph::GLSceneGraph glGraph;

    // Define grid object
    SceneGraph::GLGrid glGrid(50,2.0, true);

    // Define axis object, and set its pose
    SceneGraph::GLAxis glAxis;
    glAxis.SetPose(-1,-2,-0.1, 0, 0, M_PI/4);
    glAxis.SetScale(0.25);

    // Define 3D spiral using a line strip object
    SceneGraph::GLLineStrip glLineStrip;
    for(double t=0; t < 10*M_PI; t+= M_PI/10) {
        glLineStrip.SetPoint(cos(t)+2, sin(t)+2, -0.2*t);
    }

    // Define a mesh object and try to load model
    SceneGraph::GLMesh glMesh;
    try {
        glMesh.Init("./model.blend");
//        glMesh.SetPosition(0,0,-0.15);
        glMesh.SetPosition(0,0,-1);
        glMesh.SetScale(4.0f);
        glGraph.AddChild(&glMesh);
    }catch(exception e) {
        cerr << "Cannot load mesh. Check file exists" << endl;
    }

    SceneGraph::GLCube glCube;
//    glCube.ClearTexture();
    glCube.SetPose(1.5,1.5,-sqrt(3), M_PI/4, M_PI/4, M_PI/4);

    // Add objects to scenegraph
    glGraph.AddChild(&glGrid);
    glGraph.AddChild(&glLineStrip);
    glGraph.AddChild(&glAxis);
    glGraph.AddChild(&glCube);

    SceneGraph::GLShadowLight shadowLight(10,10,-100, 2048,2048);
    //shadowLight.SetVisible();
    shadowLight.AddShadowCaster(&glCube);
    shadowLight.AddShadowCasterAndReceiver(&glMesh);
    shadowLight.AddShadowReceiver(&glGrid);
    glGraph.AddChild(&shadowLight);

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

    pangolin::View& viewLight = pangolin::CreateDisplay()
            .SetBounds(0.0, 0.3, 0, 0.3)
            .SetDrawFunction(SceneGraph::ActivateScissorClearDrawFunctor(glGraph,shadowLight.GetRenderState()));


    // Add our view as children to the base container.
    container.AddDisplay(view3d);
    container.AddDisplay(viewLight);

    // Default hooks for exiting (Esc) and fullscreen (tab).
    for(int frame=0; !pangolin::ShouldQuit(); ++frame )
    {
        // Clear whole screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Animate position of light over time.
        shadowLight.SetPosition(100*cos(frame/100.0), 100*sin(frame/100.0), -50 );

        // Swap frames and Process Events
        pangolin::FinishGlutFrame();

        // Pause for 1/60th of a second.
        usleep(1E6 / 60);
    }

    return 0;
}
