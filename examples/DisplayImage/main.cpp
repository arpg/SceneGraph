#include <iostream>
#include <functional>
#include <chrono>
#include <thread>

#include <Eigen/Eigen>

#include <pangolin/pangolin.h>
#include <SceneGraph/SceneGraph.h>

using namespace std;

template<typename T>
void setRandomImageData(T* imageArray, int width, int height, int channels){
  for(int i = 0 ; i < channels*width*height;i++) {
    imageArray[i] = std::numeric_limits<T>::max() * ((float)rand()/RAND_MAX);
  }
}

// Draw in pixel units (center of image pixels)
struct ExampleDrawSomethingInPixelCoords
{
    void doTest( void )
    {
        pangolin::GlState glstate;
        glstate.glShadeModel(GL_FLAT);
        GLfloat verts[] = {0, 0, 0,20, 20, 20};
        GLfloat colors[] = {1,0,0,1, 0,1,0,1, 0,0,1,1};
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, verts);
        glColorPointer(4, GL_FLOAT, 0, colors);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }

    void doTest2( void )
    {
        GLfloat verts[] = {20, 20, 20,0, 0, 0};
        GLfloat colors[] = {1,0,0,1, 0,1,0,1, 0,0,1,1};
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, verts);
        glColorPointer(4, GL_FLOAT, 0, colors);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }

    void operator()(pangolin::View&) {
        glColor3f(1,1,1);
        pangolin::glDrawRectPerimeter(10,10, 40,40);
        doTest();
        doTest2();
    }
};

void GlobalKeyHook(std::string str)
{
    cout << str << endl;
}

int main( int /*argc*/, char** /*argv[]*/ )
{
    // Create OpenGL window in single line thanks to GLUT
    pangolin::CreateWindowAndBind("Main",640*2,480);
    SceneGraph::GLSceneGraph::ApplyPreferredGlSettings();
    glClearColor(0, 0, 0, 0);

    // Scenegraph to hold GLObjects and relative transformations
    SceneGraph::GLSceneGraph glGraph;
    
    SceneGraph::GLLight light(10,10,-100);
    glGraph.AddChild(&light);    

    // Define grid object
    SceneGraph::GLGrid glGrid( 50, 2.0, true );
    glGraph.AddChild(&glGrid);    

    // Define axis object, and set its pose
    SceneGraph::GLAxis glAxis;
    glAxis.SetPose(-1,-2,-1, 0, 0, M_PI/4);
    glAxis.SetScale(0.25);
    glGraph.AddChild(&glAxis);

    // Define 3D spiral using a line strip object
    SceneGraph::GLCachedPrimitives glSpiral(GL_LINE_STRIP, SceneGraph::GLColor(1.0f,0.7f,0.2f));
    for(double t=0; t < 10*M_PI; t+= M_PI/50) {
        glSpiral.AddVertex(Eigen::Vector3d(cos(t)+2, sin(t)+2, -0.1*t) );
    }
    glGraph.AddChild(&glSpiral);

    SceneGraph::GLWireSphere sphere(5);
    sphere.SetPose(5, 6, -7, M_PI / 3, M_PI / 3, M_PI / 3);
    glGraph.AddChild(&sphere);


#ifndef HAVE_GLES
    // Define 3D floating text object
    SceneGraph::GLText glText3d("3D Floating Text", -1, 1, -1);
    glGraph.AddChild(&glText3d);
#endif

    // Synthetic random image for demonstration
    const int w = 64;
    const int h = 48;
    unsigned char uImage[w*h*3];
    setRandomImageData(uImage,w,h,3);

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
    view3d.SetBounds(0.0, 1.0, 0.0, 1.0/2.0, 640.0f/480.0f)
          .SetHandler(new SceneGraph::HandlerSceneGraph(glGraph,stacks3d,pangolin::AxisNegZ))
          .SetDrawFunction(SceneGraph::ActivateDrawFunctor(glGraph, stacks3d));

    // We define a special type of view which will accept image data
    // to display and set its bounds on screen.
    SceneGraph::ImageView viewImage(true,false);
    viewImage.SetBounds(0.0, 1.0, 1.0/2.0, 1.0, (double)w/h);
    viewImage.SetDrawFunction(ExampleDrawSomethingInPixelCoords());

    // Add our views as children to the base container.
    container.AddDisplay(view3d);
    container.AddDisplay(viewImage);

    // Demonstration of how we can register a keyboard hook to trigger a method
    pangolin::RegisterKeyPressCallback( pangolin::PANGO_CTRL + 'r', std::bind(GlobalKeyHook, "You Pushed ctrl-r!" ) );

//    // Default hooks for exiting (Esc) and fullscreen (tab).
    while( !pangolin::ShouldQuit() )
    {
        // Clear whole screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // These calls can safely be made outside of the OpenGL thread.
        setRandomImageData(uImage,w,h,3);
        viewImage.SetImage(uImage, w,h, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);

        // Swap frames and Process Events
        pangolin::FinishFrame();

        // Pause for 1/60th of a second.
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
    }

    return 0;
}
