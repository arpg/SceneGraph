#include <SimpleGui/Gui.h> // simple OpenGL scene graph using FLTK
#include <SimpleGui/GetPot> // for friendy command line parsing
#include <SimpleGui/GLAxis.h>

using namespace Eigen;

GLWindow* pWin;

bool spawnCube (){
/*    const struct aiScene* pScene;
        struct aiLogStream stream = aiGetPredefinedLogStream( aiDefaultLogStream_STDOUT, NULL );
        aiAttachLogStream( &stream );
        pScene = aiImportFile( "Cube.STL", aiProcessPreset_TargetRealtime_MaxQuality );

        GLMesh cube;
        cube.Init(pScene);
        cube.SetPose(0, 0, 0, 0, 0, 0);
        pWin->AddChildToRoot(&cube);
*/
        return true;
}

int main (int argc, char** argv){
    //deals with commandline
    GetPot cl( argc, argv );

    //init window
    pWin = new GLWindow(0, 0, 1024, 768, "Axis Render Demo");

    //generate grid
    GLGrid grid;
    grid.SetPerceptable( false );
    pWin->AddChildToRoot( &grid );

    GLAxis axis1;
    pWin->AddChildToRoot( &axis1 );

    //spawnCube();

    return (pWin->Run());
}
