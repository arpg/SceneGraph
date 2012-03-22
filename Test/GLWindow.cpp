#include "GLWindow.h"
#include "GLGrid.h"
#include "GLText.h"
#include "GLMesh.h"

#include "GetPot"


////////////////////////////////////////////////////////////////////////////
int GLWindow::HandleNavInput( int e )
{
    switch( e ){
        case FL_PUSH:
            m_nMousePushX = Fl::event_x();
            m_nMousePushY = Fl::event_y();
            return 1;
        case FL_MOUSEWHEEL:
            {
                int dy = Fl::event_dy();
                // move camera along vector from CamPostion to CamTarget
                Eigen::Vector3d d = m_dCamTarget - m_dCamPosition;
                d = d / d.norm();
                if( Fl::event_ctrl() ) {
                    // up and down
                    m_dCamTarget   += -dy*m_dCamUp;
                    m_dCamPosition += -dy*m_dCamUp;
                } else {
                    m_dCamPosition = m_dCamPosition - dy*d;
                    m_dCamTarget = m_dCamPosition + d;
                }
                return 1;
            }
            break;
        case FL_DRAG:
            {
                int dx = Fl::event_x() - m_nMousePushX;
                int dy = Fl::event_y() - m_nMousePushY;
                if( Fl::event_button3() ) {
                    // "move" the world
                    double th = std::max( fabs(m_dCamPosition[2] / 100.0), 0.01 ); // move more or less depending on height
                    Eigen::Vector3d dForward = m_dCamTarget - m_dCamPosition;
                    dForward = dForward / dForward.norm();
                    Eigen::Vector3d dLR = dForward.cross(m_dCamUp);
                    // sideways
                    m_dCamTarget   += -dx * th*dLR;
                    m_dCamPosition += -dx * th*dLR;
                    // re-use "forward"
                    dForward = m_dCamUp.cross(dLR);
                    m_dCamTarget   += dy * th*dForward;
                    m_dCamPosition += dy * th*dForward;
                } else {
                    // just aim the camera -- we can write a better UI later...
                    Eigen::Vector3d dForward = m_dCamTarget - m_dCamPosition;
                    dForward = dForward / dForward.norm();

                    Eigen::Matrix3d dYaw   = GLCart2R( 0,        0, 0.001 * dx );
                    Eigen::Matrix3d dPitch = GLCart2R( 0, 0.001 * dy,        0 );

                    dForward = dYaw*dForward;
                    dForward = dPitch*dForward;

                    m_dCamTarget = m_dCamPosition + dForward;
                }
                m_nMousePushX = Fl::event_x();
                m_nMousePushY = Fl::event_y();
                return 1;
            }
            break;
        case FL_KEYBOARD:
            switch( Fl::event_key() ) {
                case '`':
                    m_Console.OpenConsole();
                    m_eGuiMode = eConsole;
                    break;
                case FL_F+1:
                    ResetCamera();
                    break;
                case FL_F+2:
                    CameraOrtho();
                    break;
				case 't':
					if( Fl::event_state(FL_CTRL) ) {
			        }
					break;

            }
            return 1;
            break;
    }
}

////////////////////////////////////////////////////////////////////////////
int GLWindow::HandleSelectionInput( int e )
{
    m_nMousePushX = Fl::event_x();
    m_nMousePushY = Fl::event_y();
    switch ( e ) {
        case FL_PUSH:
            {
                GLObject* pSelect = SelectedObject();
                if( pSelect ){
                    pSelect->select( SelectedId() );
                    m_pSelectedObject = pSelect;
                }
            }
            break;
        case FL_DRAG:
            if(m_pSelectedObject){
                m_pSelectedObject->drag();
            }
            break;
        case FL_RELEASE:
            m_eGuiMode = eFPSNav;
            if(m_pSelectedObject){
                m_pSelectedObject->release();
            }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////
int GLWindow::handle( int e )
{
    GLObject* pSelect = SelectedObject();
    if( pSelect && e == FL_PUSH ){
//        printf("%s selected\n", pSelect->ObjectName() );
        m_eGuiMode = eSelect;
	}

    switch( m_eGuiMode ){
        case eConsole:
            {
                bool res = m_Console.handle( e );
                if( !m_Console.IsOpen() ){
                    m_eGuiMode = eFPSNav;
                }
                return res;
            }
        case eFPSNav:
            return HandleNavInput( e );
        case eSelect:
            return HandleSelectionInput( e );
    }

	return false;
}


////////////////////////////////////////////////////////////////////////////
void GLWindow::Update()
{
//    printf("user update function called\n");
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

int main( int argc, char** argv )
{
    GetPot cl(argc,argv);

    std::string sMesh = cl.follow( "Ramp.STL", 1, "-mesh" );

	// init window
    GLWindow* pWin = new GLWindow( 0, 0, 1024, 768, "Ray Tracer" );



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


    // gabes cruft
    glClearColor( 0.1f,0.1f,0.1f,1.f );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );    // Uses default lighting parameters
    glEnable( GL_DEPTH_TEST );
    glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );
    glEnable( GL_NORMALIZE );
    glColorMaterial( GL_FRONT_AND_BACK, GL_DIFFUSE );


    return( Fl::run() );
}
