#include <iostream>
#include <pangolin/pangolin.h>

using namespace pangolin;
using namespace std;

int main( int /*argc*/, char** /*argv[]*/ )
{  
  // Create OpenGL window in single line thanks to GLUT
  pangolin::CreateGlutWindowAndBind("Main",640,480);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Issue specific OpenGl we might need
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_DEPTH_TEST);

  // Define Camera Render Object (for view / scene browsing)
  pangolin::OpenGlRenderState s_cam;
  s_cam.Set(ProjectionMatrix(640,480,420,420,320,240,0.1,1000));
  s_cam.Set(IdentityMatrix(GlModelViewStack));

  // Add named OpenGL viewport to window and provide 3D Handler
  View& d_cam = pangolin::CreateDisplay()
    .SetBounds(0.0, 1.0, 0.0, 1.0, -640.0f/480.0f)
    .SetHandler(new Handler3D(s_cam));

  // Default hooks for exiting (Esc) and fullscreen (tab).
  while( !pangolin::ShouldQuit() )
  {
    if(pangolin::HasResized())
      DisplayBase().ActivateScissorAndClear();

    // Activate efficiently by object
    // (3D Handler requires depth testing to be enabled)
    d_cam.ActivateScissorAndClear(s_cam);

    // Render some stuff
    glColor3f(1.0,1.0,1.0);
    glTranslatef(0,0,-3);
    glutWireTeapot(1.0);

    // Swap frames and Process Events
    pangolin::FinishGlutFrame();
  }

  return 0;
}
