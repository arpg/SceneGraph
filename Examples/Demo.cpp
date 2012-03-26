#include <SimpleGui/Gui.h> // simple OpenGL scene graph using FLTK 
#include <SimpleGui/GetPot> // for friendy command line parsing

int main( int argc, char** argv )
{
    glutInit(&argc, argv);
    GetPot cl(argc,argv);

    std::string sMesh = cl.follow( "Terrain.ac", 1, "-mesh" );

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

    // GL
    {
        glClearColor( 0.1f,0.1f,0.1f,1.f );
        glEnable( GL_LIGHTING );
        glEnable( GL_LIGHT0 );    // Uses default lighting parameters
        glEnable( GL_DEPTH_TEST );
        glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );
        glEnable( GL_NORMALIZE );
        glColorMaterial( GL_FRONT_AND_BACK, GL_DIFFUSE );

        glShadeModel(GL_SMOOTH);                    // shading mathod: GL_SMOOTH or GL_FLAT

        // enable /disable features
        glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
        glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
        glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
        glEnable( GL_DEPTH_TEST );
        glEnable( GL_LIGHTING );
        glEnable( GL_TEXTURE_RECTANGLE_ARB );

        // track material ambient and diffuse from surface color, 
        // call it before glEnable(GL_COLOR_MATERIAL)
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);

        glClearColor(0, 0, 0, 0);    // background color
        glClearStencil(0);           // clear stencil buffer
        glClearDepth(1.0f);          // 0 is near, 1 is far
        glDepthFunc(GL_LEQUAL);
    }

	// register objects
    pWin->AddChildToRoot( &mesh );
	pWin->AddChildToRoot( &grid );

    return( pWin->Run() );
}

