#include <iostream>
#include <pangolin/pangolin.h>
#include <SceneGraph/GLSceneGraph.h>
#include <SceneGraph/GLMesh.h>
#include <SceneGraph/GLGrid.h>

using namespace SceneGraph;
using namespace pangolin;
using namespace std;

void SetupOpenGL()
{
    glShadeModel(GL_SMOOTH);

    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    GLfloat light_pos[] = {0,0,0.01};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos );

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_NORMALIZE);

    glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
    glEnable( GL_COLOR_MATERIAL );

    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
    glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );

    glDepthFunc( GL_LEQUAL );
    glEnable( GL_DEPTH_TEST );

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

int main( int /*argc*/, char** /*argv[]*/ )
{  
  // Create OpenGL window in single line thanks to GLUT
  pangolin::CreateGlutWindowAndBind("Main",640,480);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  SetupOpenGL();

  // Define Camera Render Object (for view / scene browsing)
  pangolin::OpenGlRenderState renderState;
  renderState.Set(ProjectionMatrix(640,480,420,420,320,240,0.1,1000));
  renderState.Set(IdentityMatrix(GlModelViewStack));

  // Add named OpenGL viewport to window and provide 3D Handler
  View& v3d = pangolin::CreateDisplay()
    .SetBounds(0.0, 1.0, 0.0, 1.0, -640.0f/480.0f)
    .SetHandler(new Handler3D(renderState,AxisNegZ));

  GLSceneGraph glGraph;
  GLMesh glCar("beatle-no-wheels-no-interior.blend");
  glGraph.AddChild(&glCar);
  GLGrid glGrid;
  glGraph.AddChild(&glGrid);

  // Default hooks for exiting (Esc) and fullscreen (tab).
  while( !pangolin::ShouldQuit() )
  {
    v3d.ActivateScissorAndClear(renderState);

    // Render some stuff
    glTranslatef(0,0,-3);
    glColor3f(1.0,1.0,1.0);
    glGraph.draw();

    // Swap frames and Process Events
    glutSwapBuffers();
    glutMainLoopEvent();

//    pangolin::FinishGlutFrame();
    usleep(1000);
  }

  return 0;
}
