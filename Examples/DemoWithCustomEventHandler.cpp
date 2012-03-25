#include <SimpleGui/Gui.h> // simple OpenGL scene graph using FLTK 
#include "GetPot" // for friendy command line parsing

// Compare to Demo.cpp

/// example of how to handle fltk events in a sub-class
class GuiWindow: public GLWindow
{

public:
    GuiWindow(int x,int y,int w,int h,const char *l=0 ) : GLWindow(x,y,w,h,l)
    {
    }

private:

    ////////////////////////////////////////////////////////////////////////////
    virtual int handle( int e )
    {
        // Here we let GLWindow::SimpleDefaultEventHandler handle the event...
        // Typically you would replace this with your own logic (though
        // SimpleDefaultEventHandler is a starting point that can serve as a
        // nice exampe.
        return SimpleDefaultEventHandler( e ); 
    }
};


////////////////////////////////////////////////////////////////////////////

int main( int argc, char** argv )
{
    GetPot cl(argc,argv);

    std::string sMesh = cl.follow( "Ramp.STL", 1, "-mesh" );

	// init window
    GuiWindow* pWin = new GuiWindow( 0, 0, 1024, 768, "Simple Gui Demo" );

    // load mesh
    const struct aiScene* pScene;
    struct aiLogStream stream = aiGetPredefinedLogStream( aiDefaultLogStream_STDOUT, NULL );
    aiAttachLogStream( &stream );
    pScene = aiImportFile( sMesh.c_str(), aiProcessPreset_TargetRealtime_MaxQuality );

    GLMesh mesh;
    GLGrid grid;

    mesh.Init( pScene );

	// register objects
	pWin->AddChildToRoot( &grid );
    pWin->AddChildToRoot( &mesh );

    return( pWin->Run() );
}
