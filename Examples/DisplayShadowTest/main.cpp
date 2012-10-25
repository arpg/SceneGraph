#include <iostream>
#include <boost/bind.hpp>
#include <Eigen/Eigen>

#include <pangolin/pangolin.h>
#include <pangolin/gl.h>
#include <SceneGraph/SceneGraph.h>

using namespace std;

int main( int /*argc*/, char** /*argv[]*/ )
{
    // Create OpenGL window in single line thanks to GLUT
    pangolin::CreateGlutWindowAndBind("Main",640,480);
    SceneGraph::GLSceneGraph::ApplyPreferredGlSettings();
    glewInit();
    glClearColor(0,0,0,1);

    // Offscreen framebuffer
    pangolin::GlTexture fb_img(640,480);
    pangolin::GlRenderBuffer fb_depth(640,480);
    pangolin::GlFramebuffer framebuffer(fb_img,fb_depth);

    // Scenegraph to hold GLObjects and relative transformations
    SceneGraph::GLSceneGraph glGraph;
    glGraph.AddLight(Eigen::Vector3d(0,0,-10));

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
        glMesh.SetPosition(0,0,-0.15);
        glGraph.AddChild(&glMesh);
    }catch(exception e) {
        cerr << "Cannot load mesh. Check file exists" << endl;
    }

    SceneGraph::GLCube glCube;
    glCube.SetTexture(fb_img.tid);
    glCube.SetPose(0,0,-sqrt(3), M_PI/4, M_PI/4, M_PI/4);

    // Add objects to scenegraph
    glGraph.AddChild(&glGrid);
    glGraph.AddChild(&glLineStrip);
    glGraph.AddChild(&glAxis);
    glGraph.AddChild(&glCube);

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

    // Add our view as children to the base container.
    container.AddDisplay(view3d);

    // Default hooks for exiting (Esc) and fullscreen (tab).
    while( !pangolin::ShouldQuit() )
    {
        // Clear whole screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render view of scene to framebuffer
        framebuffer.Bind();
        glViewport(0,0,640,480);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        stacks3d.Apply();
        glGraph.DrawObjectAndChildren();
        framebuffer.Unbind();

        // Swap frames and Process Events
        pangolin::FinishGlutFrame();

        // Pause for 1/60th of a second.
        usleep(1E6 / 60);
    }

    return 0;
}
