#include <SimpleGui/Gui.h> // simple OpenGL scene graph using FLTK
#include <SimpleGui/GetPot> // for friendy command line parsing

#include <SimpleGui/GLAxis.h>

using namespace Eigen;

int main (int argc, char** argv){
    //deals with commandline
    GetPot cl( argc, argv );

    //init window
    GLWindow* pWin = new GLWindow(0, 0, 1024, 768, "Axis Render Demo");

    //generate grid
    GLGrid grid;
    grid.SetPerceptable( false );
    pWin->AddChildToRoot( &grid );

    GLAxis axis1;
    pWin->AddChildToRoot( &axis1 );

    return (pWin->Run());
}
