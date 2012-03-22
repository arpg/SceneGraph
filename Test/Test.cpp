
#include "GLWindow.h"
#include "GLGrid.h"
#include "GLText.h"
#include "GLMesh.h"
#include "GetPot"


////////////////////////////////////////////////////////////////////////////

int main( int argc, char** argv )
{
    GetPot cl(argc,argv);

    std::string sMesh = cl.follow( "Ramp.STL", 1, "-mesh" );

	// init window
    GLWindow* pWin = new GLWindow( 0, 0, 1024, 768, "Simple Gui Demo" );

    // load mesh
    const struct aiScene* pScene;
    struct aiLogStream stream = aiGetPredefinedLogStream( aiDefaultLogStream_STDOUT, NULL );
    aiAttachLogStream( &stream );
    pScene = aiImportFile( sMesh.c_str(), aiProcessPreset_TargetRealtime_MaxQuality );

    GLMesh mesh;
    GLGrid grid;

    mesh.Init( pScene );

	// register objects
	pWin->RegisterObject( &grid );
    pWin->RegisterObject( &mesh );

    return( pWin->Run() );
}
