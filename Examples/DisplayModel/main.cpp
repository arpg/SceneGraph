#include <iostream>
#include <boost/bind.hpp>
#include <Eigen/Eigen>

#include <pangolin/pangolin.h>
#include <SceneGraph/SceneGraph.h>

using namespace SceneGraph;
using namespace pangolin;
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
    void operator()(pangolin::View&) {
        glColor3f(1,1,1);
        glBegin(GL_LINE_STRIP);
        glVertex2d(10, 10);
        glVertex2d(40, 10);
        glVertex2d(40, 40);
        glVertex2d(10, 40);
        glVertex2d(10, 10);
        glEnd();
    }
};

void GlobalKeyHook(std::string str)
{
    cout << str << endl;
}

int main( int /*argc*/, char** /*argv[]*/ )
{
    // Create OpenGL window in single line thanks to GLUT
    pangolin::CreateGlutWindowAndBind("Main",640*3,480);
    GLSceneGraph::ApplyPreferredGlSettings();

    // Scenegraph to hold GLObjects and relative transformations
    GLSceneGraph glGraph;

    // Define grid object
    GLGrid glGrid(50,2.0, true);

    // Define axis object, and set its pose
    GLAxis glAxis;
    glAxis.SetPose(-1,-2,-0.1, 0, 0, M_PI/4);
    glAxis.SetScale(0.25);

    // Define movable waypoint object with velocity
    GLWayPoint glWaypoint;
    glWaypoint.SetPose(0.5,0.5,-0.1,0,0,0);

    // Define 3D spiral using a line strip object
    GLLineStrip glLineStrip;
    for(double t=0; t < 10*M_PI; t+= M_PI/10) {
        glLineStrip.SetPoint(cos(t)+2, sin(t)+2, -0.2*t);
    }

    // Define 3D floating text object
    GLText glText3d("3D Floating Text", -1, 1, -1);

    // Define a mesh object and try to load model
    GLMesh glMesh;
    try {
        glMesh.Init("./model.blend");
        glMesh.SetPosition(0,0,-0.15);
        glGraph.AddChild(&glMesh);
    }catch(exception e) {
        cerr << "Cannot load mesh. Check file exists" << endl;
    }

    // Add objects to scenegraph
    glGraph.AddChild(&glGrid);
    glGraph.AddChild(&glWaypoint);
    glGraph.AddChild(&glLineStrip);
    glGraph.AddChild(&glAxis);
    glGraph.AddChild(&glText3d);

    // We can have more than one scenegraph. This 2d scenegraph will
    // be rendered with an orthographic projection. This is useful
    // for text overlays.
    GLSceneGraph glGraph2d;

    GLText glText2d("2D Overlay Text", 10, 460);
    glGraph2d.AddChild(&glText2d);

    // Synthetic random image for demonstration
    const int w = 64;
    const int h = 48;
    unsigned char uImage[w*h*3];
    setRandomImageData(uImage,w,h,3);

    // Define Camera Render Object (for view / scene browsing)
    pangolin::OpenGlRenderState stacks3d(
        ProjectionMatrix(640,480,420,420,320,240,0.1,1000),
        ModelViewLookAt(0,-2,-4, 0,1,0, AxisZ)
    );

    // Define second camera render object
    pangolin::OpenGlRenderState stacks3d_2(
        ProjectionMatrix(640,480,420,420,320,240,0.1,1000),
        ModelViewLookAt(2,-1,-2, 0,1,0, AxisNegZ)
    );

    // Use different orthographic render state for 2D drawing.
    // Virtualise screen resolution to 640x480
    pangolin::OpenGlRenderState stacks2d(
        ProjectionMatrixOrthographic(0,640,0,480,0,1000)
    );

    // Pangolin abstracts the OpenGL viewport as a View.
    // Here we get a reference to the default 'base' view.
    pangolin::View& container = pangolin::DisplayBase();

    // We define a new view which will reside within the container.
    pangolin::View view3d;

    // We set the views location on screen and add a handler which will
    // let user input update the model_view matrix (stacks3d) and feed through
    // to our scenegraph
    view3d.SetBounds(0.0, 1.0, 0.0, 1.0/3.0, 640.0f/480.0f)
          .SetHandler(new HandlerSceneGraph(glGraph,stacks3d,AxisZ))
          .SetDrawFunction(ActivateDrawFunctor(glGraph, stacks3d));

    // We can define another view on the same scenegraph. We can also
    // render a second scenegraph as a 2D overlay using the different
    // 2D ModelView and Projection matrices (stacks3d)
    pangolin::View view3d_2d;
    view3d_2d.SetBounds(0.0,1.0, 2.0/3.0, 1.0, 640.0f/480.0f)
          .SetHandler(new HandlerSceneGraph(glGraph,stacks3d_2,AxisNegZ))
          .SetDrawFunction(ActivateDrawFunctor3d2d(glGraph, stacks3d_2, glGraph2d, stacks2d));

    // We define a special type of view which will accept image data
    // to display and set its bounds on screen.
    ImageView viewImage(true,false);
    viewImage.SetBounds(0.0, 1.0, 1.0/3.0, 2.0/3.0, (double)w/h);
    viewImage.SetDrawFunction(ExampleDrawSomethingInPixelCoords());

    // Add our views as children to the base container.
    container.AddDisplay(view3d);
    container.AddDisplay(view3d_2d);
    container.AddDisplay(viewImage);

    // Demonstration of how we can register a keyboard hook to trigger a method
    pangolin::RegisterKeyPressCallback( PANGO_CTRL + 'r', boost::bind(GlobalKeyHook, "You Pushed ctrl-r!" ) );

    // Default hooks for exiting (Esc) and fullscreen (tab).
    while( !pangolin::ShouldQuit() )
    {
        // Clear whole screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // These calla can safely be made outside of the OpenGL thread.
        setRandomImageData(uImage,w,h,3);
        viewImage.SetImage(uImage, w,h, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE);

        // Swap frames and Process Events
        pangolin::FinishGlutFrame();

        // Pause for 1/60th of a second.
        usleep(1E6 / 60);
    }

    return 0;
}
