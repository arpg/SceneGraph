#include <iostream>
#include <chrono>
#include <thread>
#include <Eigen/Eigen>

#include <SceneGraph/SceneGraph.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

using namespace std;

void error_callback(int error, const char* description)
{
  fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
}

template<typename T>
void setRandomImageData(T* imageArray, int width, int height, int channels){
  for(int i = 0 ; i < channels*width*height;i++) {
    imageArray[i] = std::numeric_limits<T>::max() * ((float)rand()/RAND_MAX);
  }
}

// the object that defines what a user sees
class View3d
{
  public:
    View3d( GLFWwindow* window ) : window_( window )
    {
      glfwGetFramebufferSize(window_, &width_, &height_);
      glViewport(0,0,width_,height_);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      SceneGraph::Perspective(
          45.0f,(GLfloat)width_/(float)height_,0.1f,100.0f);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
    };

    void SetPerspective( 
        float fovxy, float zNear, float zFar)
    {
      float aspect = (float)width_/(float)height_;
      near_ = zNear;
      far_ =  zFar;
      fovxy_ = fovxy;

      float xmin, xmax, ymin, ymax;
      ymax = zNear * tan(fovxy * M_PI / 360.0);
      ymin = -ymax;
      xmin = ymin * aspect;
      xmax = ymax * aspect;
      glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
    }

    template <typename T=float>
      void SetPose( const Eigen::Matrix<T,4,4>& m  )
      {
        pose_ = m;
      }

    template <typename T=float>
      void SetPose( const Eigen::Matrix<T,6,1>& xyzpqr  )
      {
        pose_ = SceneGraph::GLCart2T( xyzpqr );
      }

    template <typename T=float>
      void SetPosition( const Eigen::Matrix<T,3,1>& t )
      {
        pose_.block<3,1>(0,3) = t;
      }

    template <typename T=float>
      void LookAt(
          T targetx,
          T targety,
          T targetz
          )
      {

      }

    template <typename T=float>
      void LookAt(
          T eyex,
          T eyey,
          T eyez,
          T targetx,
          T targety,
          T targetz,
          T upx,
          T upy,
          T upz
          )
      {
        SceneGraph::sgLookAt( eyex, eyey, eyez, targetx, targety,
            targetz, upx, upy, upz );
      }

    float near_;
    float far_;
    float fovxy_;
    int width_;
    int height_;
    Eigen::Matrix4f pose_;
    GLFWwindow* window_;
};


/*
class View2d
{
  public:
    View2d( GLFWwindow* window ) : window_( window )
    {
      glfwGetFramebufferSize(window_, &width_, &height_);
      glViewport(0,0,width_,height_);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
    };

    void SetSize( int top, int left, int with, height )
    {

    }

    float near_;
    float far_;
    float fovxy_;
    int width_;
    int height_;
    Eigen::Matrix4f pose_;
    GLFWwindow* window_;
};
*/


GLFWwindow* GuiWindow( int w, int h, const char* name )
{
  GLFWwindow* window;
  glfwSetErrorCallback(error_callback);
  if (!glfwInit()){
    exit(EXIT_FAILURE);
  }
  window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  glfwSetKeyCallback(window, key_callback);
  return window;
}

double ellapsedTime()
{
  static double prev_time = glfwGetTime();
  double cur_time = glfwGetTime();
  double dt = cur_time-prev_time; 
  prev_time = cur_time;
  return dt; 
}

int main( int /*argc*/, char** /*argv[]*/ )
{
  // setup GL window
//  GLFWwindow* window = GuiWindow( 640, 480, "Simple Demo" );
  GLFWwindow* window;
  glfwSetErrorCallback(error_callback);
  if (!glfwInit()){
    exit(EXIT_FAILURE);
  }
  window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  glfwSetKeyCallback(window, key_callback);

  // Scenegraph to hold GLObjects and relative transformations
  SceneGraph::GLCube cube;
  SceneGraph::GLGrid grid( 10, 1, true ) ;
  SceneGraph::GLCylinder cylinder;
  cylinder.SetPosition( 0,0, -4 ); 
  SceneGraph::GLAxis axis;
  SceneGraph::GLLight light;
  SceneGraph::GLImage image;

  // Synthetic random image for demonstration
  const int w = 64;
  const int h = 48;
  unsigned char uImage[w*h*3];
  setRandomImageData(uImage,w,h,3);
  image.SetImage( uImage, w, h, GL_RGB, GL_UNSIGNED_BYTE );

  // add all these objects to a scene graph
  SceneGraph::GLSceneGraph graph;
  graph.AddChild( &grid );
  graph.AddChild( &cube );
  graph.AddChild( &cylinder );
  graph.AddChild( &light );
  graph.ApplyPreferredGlSettings();

  View3d view( window );
  view.SetPerspective( 45.0f, 0.1, 100 );
  view.LookAt( -5,5,-5, 0,0,0, 0,0,-1 );

  while (!glfwWindowShouldClose(window)) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // position guide the "camera"
    view.LookAt( -5,5,-5, 0,0,0, 0,0,-1 );

     // These calls can safely be made outside of the OpenGL thread.
    setRandomImageData(uImage,w,h,3);
    image.SetImage(uImage, w,h, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);

    // roll the cube
    Eigen::Vector6d p = cube.GetPose();
    p(3) += ellapsedTime();
    cube.SetPose( p );

    // draw scene graph
    graph.DrawObjectAndChildren();

    // swap buffers and handle input
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
/*    
    if (!glfwInit()){
      exit(EXIT_FAILURE);
      }
    
    // Define 3D spiral using a GLCachedPrimitives object
    SceneGraph::GLCachedPrimitives glSpiral(GL_LINE_STRIP, SceneGraph::GLColor(1.0f,0.7f,0.2f) );
    for(double t=0; t < 10*M_PI; t+= M_PI/50) {
        glSpiral.AddVertex(Eigen::Vector3d(cos(t)+2, sin(t)+2, -0.1*t) );
    }
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
    glWaypoint.SetPose(0.5,0.5,-0.1,0,0,0);
    glGraph.AddChild(&glWaypoint);

    // Optionally clamp waypoint to specific plane
    glWaypoint.ClampToPlane(Eigen::Vector4d(0,0,1,0));
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
    */
}
