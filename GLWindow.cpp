#include "GLWindow.h"
#include "GLGrid.h"
#include "GLText.h"
#include "GLLineStrip.h"
#include "GLCar.h"
#include "GLWayPoint.h"
#include "GLRamp.h"
#include "GLMesh.h"


#include "GetPot"

#include <CarPlanner.h>

#include <CVars/CVarVectorIO.h>
#include <Fl/Enumerations.H>

#include "AssetLoader.h"

using namespace CVarUtils;


////////////////////////////////////////////////////////////////////////////
// Hermes node
#include "Node.h"
#include "Messages.pb.h"

rpg::Node			gNode(5555);
std::string         g_sMochaPubSubUri;
std::string         g_sMochaRpcUri;

////////////////////////////////////////////////////////////////////////////
// Overloading Eigen for CVars
std::ostream& operator<<( std::ostream& Stream, Eigen::Vector5d& Mat ) {
	unsigned int nRows = Mat.rows();
	unsigned int nCols = Mat.cols();

	Stream << "[ ";

	for( unsigned int ii = 0; ii < nRows-1; ii++ ) {
		for( unsigned int jj = 0; jj < nCols-1; jj++ ) {
			Stream << Mat(ii, jj);
			Stream << ", ";
		}
		Stream << Mat(ii, nCols-1);
		Stream << "; ";
	}
	for( unsigned int jj = 0; jj < nCols-1; jj++ ) {
		Stream << Mat(nRows-1, jj);
		Stream << ", ";
	}
	Stream << Mat(nRows-1, nCols-1);
	Stream << " ]";

	return Stream;
}

std::istream& operator>>( std::istream& Stream, Eigen::Vector5d& Mat ) {

	unsigned int nRows = Mat.rows();
	unsigned int nCols = Mat.cols();
	char str[256];

	Stream.getline(str, 255, '[');
	if( Stream.gcount() > 1 ) {
		return Stream;
	}
	for( unsigned int ii = 0; ii < nRows-1; ii++ ) {
		for( unsigned int jj = 0; jj < nCols-1; jj++ ) {
			Stream.getline(str, 255, ',');
			Mat(ii, jj) = std::strtod(str, NULL);
		}
		Stream.getline(str, 255, ';');
		Mat(ii, nCols-1) = std::strtod(str, NULL);
	}
	for( unsigned int jj = 0; jj < nCols-1; jj++ ) {
		Stream.getline(str, 255, ',');
		Mat(nRows-1, jj) = std::strtod(str, NULL);
	}
	Stream.getline(str, 255, ']');
	Mat(nRows-1, nCols-1) = std::strtod(str, NULL);
	return Stream;
}


std::vector<Eigen::Vector5d*>	gWayPoints;
std::vector<int>&				gPath = CVarUtils::CreateCVar( "path", std::vector<int>(), "Path vector.");

////////////////////////////////////////////////////////////////////////////
// Global CVars
bool _SetWaypoints( std::vector<std::string> *vArgs )
{
	Waypoints ReqMsg;
	RetVal RepMsg;
	for( int ii = 0; ii < gWayPoints.size(); ii++ ) {
		Waypoint* WpntMsg = ReqMsg.add_wpoints();
		Eigen::Vector5d& Wpnt = *(gWayPoints[ii]);
		WpntMsg->set_x(Wpnt(0));
		WpntMsg->set_y(Wpnt(1));
		WpntMsg->set_theta(Wpnt(2));
		WpntMsg->set_velocity(Wpnt(3));
		WpntMsg->set_w(Wpnt(4));
	}
	FLConsoleInstance* pConsole = GetConsole();
	if( gNode.Call( g_sMochaRpcUri, "Waypoints", ReqMsg, ReqMsg) == false ) {
		pConsole->Printf("Error calling RPC.");
		return false;
	}
	if( RepMsg.val() == true ) {
		pConsole->Printf("Value set.");
	} else {
		pConsole->PrintError("Error setting waypoints.");
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////
bool _SetPath( std::vector<std::string> *vArgs )
{
	Path ReqMsg;
	RetVal RepMsg;
	for( int ii = 0; ii < gPath.size(); ii++ ) {
		ReqMsg.add_indices( gPath[ii] );
	}
	FLConsoleInstance* pConsole = GetConsole();
	if( gNode.Call( g_sMochaRpcUri, "Path", ReqMsg, ReqMsg) == false ) {
		pConsole->Printf("Error calling RPC.");
		return false;
	}
	if( RepMsg.val() == true ) {
		pConsole->Printf("Value set.");
	} else {
		pConsole->PrintError("Error setting path indices.");
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////
bool _MochaStart( std::vector<std::string> *vArgs )
{
	RetVal ReqMsg,RepMsg;
	ReqMsg.set_val(true);
	FLConsoleInstance* pConsole = GetConsole();
	if( gNode.Call( g_sMochaRpcUri, "Start", ReqMsg, ReqMsg) == false ) {
		pConsole->Printf("Error calling RPC.");
		return false;
	}
	if( RepMsg.val() ) {
		pConsole->Printf("Mochaccino started...");
	} else {
		pConsole->PrintError("Error starting Mochaccino.");
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////
bool _MochaStop( std::vector<std::string> *vArgs )
{
	RetVal ReqMsg,RepMsg;
	ReqMsg.set_val(false);
	FLConsoleInstance* pConsole = GetConsole();
	if( gNode.Call( g_sMochaRpcUri, "Stop", ReqMsg, ReqMsg) == false ) {
		pConsole->Printf("Error calling RPC.");
		return false;
	}
	if( RepMsg.val() ) {
		pConsole->Printf("Mochaccino stopped...");
	} else {
		pConsole->PrintError("Error stopping Mochaccino.");
	}
	return true;
}



Eigen::Vector5d& g_Waypnt0 = CVarUtils::CreateCVar( "waypnt.0", Eigen::Vector5d());
Eigen::Vector5d& g_Waypnt1 = CVarUtils::CreateCVar( "waypnt.1", Eigen::Vector5d());
Eigen::Vector5d& g_Waypnt2 = CVarUtils::CreateCVar( "waypnt.2", Eigen::Vector5d());
Eigen::Vector5d& g_Waypnt3 = CVarUtils::CreateCVar( "waypnt.3", Eigen::Vector5d());
Eigen::Vector5d& g_Waypnt4 = CVarUtils::CreateCVar( "waypnt.4", Eigen::Vector5d());
Eigen::Vector5d& g_Waypnt5 = CVarUtils::CreateCVar( "waypnt.5", Eigen::Vector5d());
Eigen::Vector5d& g_Waypnt6 = CVarUtils::CreateCVar( "waypnt.6", Eigen::Vector5d());
Eigen::Vector5d& g_Waypnt7 = CVarUtils::CreateCVar( "waypnt.7", Eigen::Vector5d());
Eigen::Vector5d& g_Waypnt8 = CVarUtils::CreateCVar( "waypnt.8", Eigen::Vector5d());
Eigen::Vector5d& g_Waypnt9 = CVarUtils::CreateCVar( "waypnt.9", Eigen::Vector5d());


////////////////////////////////////////////////////////////////////////////
// GL Object List
std::vector<GLLineStrip>		gGLLineSegments;
std::vector<GLWayPoint>			gGLWayPoints;
GLGrid							gGLGrid;
GLText							gGLLoc;
GLText							gGLLocType;
GLCar							gGLCar;
GLCar							gGLDesCar;
GLRamp                          gGLRamp;

////////////////////////////////////////////////////////////////////////////
// Car planner for trajectory plotting
CarPlanner		gPlanner;



////////////////////////////////////////////////////////////////////////////
void Sample3DPath(
	const Eigen::Vector5d xi,
	const Eigen::Vector5d xf,
	const unsigned int nSegs,
	std::vector<double>& vPts
	)
{
	std::vector<double> v2dPts;
	gPlanner.SetGoal( 0, xi, xf );
	gPlanner.SamplePath( nSegs, v2dPts );

	vPts.clear();
	for( size_t ii = 0; ii < v2dPts.size(); ii += 2 ) {
		double x = v2dPts[ii];
		double y = v2dPts[ii + 1];
		double z = 0;
		vPts.push_back( x );
		vPts.push_back( y );
		vPts.push_back( z );
	}
}

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
						_SetPath(NULL);
						_SetWaypoints(NULL);
						_MochaStart(NULL);
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
			_SetWaypoints(NULL);
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
	PublishMsg pbMsg;

	// clean buffer until last message is read
	if( gNode.Read( "WorldState", pbMsg ) == true ) {

		while( gNode.Read( "WorldState", pbMsg ) == true ) {}

		// update car pose
		Entity* pbEntity = pbMsg.mutable_name(0);
		Pose pbPose = pbEntity->pose();
		Eigen::Matrix<double,6,1> Vec;
		Vec(0) = pbPose.x();
		Vec(1) = pbPose.y();
		Vec(2) = pbPose.z();
		Vec(3) = pbPose.roll();
		Vec(4) = pbPose.pitch();
		Vec(5) = pbPose.yaw();
        char tBuff[100] = {};
        sprintf(tBuff, "( X: %.2f, Y: %.2f, Z: %.2f)", Vec(0), Vec(1), Vec(2));
        gGLLoc.SetText(tBuff);
		gGLCar.SetPose(Vec);

        // update desired pose
        pbEntity = pbMsg.mutable_name(1);
		pbPose = pbEntity->pose();
		Vec(0) = pbPose.x();
		Vec(1) = pbPose.y();
		Vec(2) = pbPose.z();
		Vec(3) = pbPose.roll();
		Vec(4) = pbPose.pitch();
		Vec(5) = pbPose.yaw();
		gGLDesCar.SetPose(Vec);

        // update ramps
        pbEntity = pbMsg.mutable_name(2);
		pbPose = pbEntity->pose();
		Vec(0) = pbPose.x();
		Vec(1) = pbPose.y();
		Vec(2) = pbPose.z();
		Vec(3) = pbPose.roll();
		Vec(4) = pbPose.pitch();
		Vec(5) = pbPose.yaw();
		gGLRamp.SetPose(Vec);

		// update air-ground state
        gGLLocType.SetText(pbMsg.loctype());

	}

	// hide everything
	for( int ii = 0; ii < gGLWayPoints.size(); ii++ ) {
		gGLWayPoints[ii].SetInVisible();
	}
	for( int ii = 0; ii < gPath.size(); ii++ ) {
		gGLLineSegments[ii].SetInVisible();
	}

	// turn the way points on
	for( int ii = 0; ii < gPath.size(); ii++ ) {
		gGLWayPoints[ii].SetWayPoint( gPath[ii], *gWayPoints[gPath[ii]] );
		gGLWayPoints[ii].SetVisible();
	}

	// now add line segments
	for( int ii = 0; ii < gPath.size() - 1; ii++ ) {
		int nStartIdx = gPath[ii];
		int nEndIdx = gPath[ii + 1];
		Eigen::Vector5d& a = *gWayPoints[nStartIdx];
		Eigen::Vector5d& b = *gWayPoints[nEndIdx];
		std::vector<double> v3dPts;
		Sample3DPath( a, b, 50, v3dPts );
		gGLLineSegments[ii].SetPoints( v3dPts );
		gGLLineSegments[ii].SetVisible();
	}
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

GLMesh gMesh;

int main( int argc, char** argv )
{
    GetPot cl(argc,argv);

    std::string sRamp = cl.follow( "Ramp.x", 1, "-ramp" );
    std::string sHost = cl.follow( "192.168.10.2", 2, "-h", "--host" );
    std::string sPubSubPort = cl.follow( "6667", 2, "-pp", "--pub_sub_port" );
    std::string sRpcPort = cl.follow( "6666", 2, "-rp", "--rpc_port" );
    g_sMochaRpcUri = sHost + ":" + sRpcPort;
    g_sMochaPubSubUri = sHost + ":" + sPubSubPort;

	// init window
    GLWindow* pWin = new GLWindow( 0, 0, 1024, 768, "MochaGUI" );

	CVarUtils::CreateCVar( "setWaypoints", _SetWaypoints, "Set waypoints." );
	CVarUtils::CreateCVar( "setPath", _SetPath, "Set path." );
	CVarUtils::CreateCVar( "mochaStart", _MochaStart, "Start engine." );
	CVarUtils::CreateCVar( "mochaStop", _MochaStop, "Stop engine." );


	gWayPoints.push_back( &g_Waypnt0 );
	gWayPoints.push_back( &g_Waypnt1 );
	gWayPoints.push_back( &g_Waypnt2 );
	gWayPoints.push_back( &g_Waypnt3 );
	gWayPoints.push_back( &g_Waypnt4 );
	gWayPoints.push_back( &g_Waypnt5 );
	gWayPoints.push_back( &g_Waypnt6 );
	gWayPoints.push_back( &g_Waypnt7 );
	gWayPoints.push_back( &g_Waypnt8 );
	gWayPoints.push_back( &g_Waypnt9 );


	// actual sequence of waypoints we will follow -- change this and you change the loop
	gPath.push_back(0);
	gPath.push_back(1);
	gPath.push_back(2);
	gPath.push_back(3);
	gPath.push_back(0);

	// subscribe to feeder
	gNode.Subscribe( "WorldState", g_sMochaPubSubUri );

	// register objects
	pWin->RegisterObject( &gGLGrid );

	pWin->RegisterObject( &gGLLoc );
	pWin->RegisterObject( &gGLLocType );
	pWin->RegisterObject( &gGLCar );
	pWin->RegisterObject( &gGLDesCar );
	pWin->RegisterObject( &gGLRamp );

    // load mesh
    const struct aiScene* pScene;
//    std::string sName = "ramp.wrl";
    //std::string sName = "Ramp.raw";
    struct aiLogStream stream = aiGetPredefinedLogStream( aiDefaultLogStream_STDOUT, NULL );
    aiAttachLogStream( &stream );
    std::string sName = sRamp;
    pScene = aiImportFile( sName.c_str(), aiProcessPreset_TargetRealtime_MaxQuality );


    gMesh.Init( pScene );

    pWin->RegisterObject( &gMesh );


	gGLWayPoints.resize( gWayPoints.size() );
    for( int ii = 0; ii < gGLWayPoints.size(); ii++ ) {
		gGLWayPoints[ii].SetWayPoint( gPath[ii], *gWayPoints[ii] );
        pWin->RegisterObject( &gGLWayPoints[ii] );
    }

	gGLLineSegments.resize( gWayPoints.size()-1 );
	for( int ii = 0; ii < gGLLineSegments.size(); ii++ ) {
		pWin->RegisterObject( &gGLLineSegments[ii] );
	}

	// init objects
	gGLLoc.InitReset();
	gGLLoc.SetPos(10,20);
	gGLLoc.SetText("X: 0.0,  Y: 0.0,  Z: 0.0");
	gGLLocType.InitReset();
	gGLLocType.SetPos(10,38);
	gGLLocType.SetText("Ground");
	gGLCar.InitReset();
	gGLCar.SetScale(0.4);
	gGLCar.SetColor(GLColor(0,0,255));
	gGLDesCar.InitReset();
	gGLDesCar.SetScale(0.4);
	gGLDesCar.SetColor(GLColor(0,255,0));
	gGLRamp.InitReset();


	// init waypoints
	g_Waypnt0 << 0, 0, 0, 0, 0;
	g_Waypnt1 << 0, 0, 0, 0, 0;
	g_Waypnt2 << 0, 0, 0, 0, 0;
	g_Waypnt3 << 0, 0, 0, 0, 0;
	g_Waypnt4 << 0, 0, 0, 0, 0;
	g_Waypnt5 << 0, 0, 0, 0, 0;
	g_Waypnt6 << 0, 0, 0, 0, 0;
	g_Waypnt7 << 0, 0, 0, 0, 0;
	g_Waypnt8 << 0, 0, 0, 0, 0;
	g_Waypnt9 << 0, 0, 0, 0, 0;

	g_Waypnt0 << 0, 0, 0, 1, 0;
	g_Waypnt1 << 6, 0,  M_PI/2, 1, 0;
	g_Waypnt2 << 5, 6,  M_PI,   1, 0;
	g_Waypnt3 << 0, 5, 3*M_PI/2, 1, 0;


    glClearColor( 0.1f,0.1f,0.1f,1.f );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );    // Uses default lighting parameters
    glEnable( GL_DEPTH_TEST );
    glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );
    glEnable( GL_NORMALIZE );
    glColorMaterial( GL_FRONT_AND_BACK, GL_DIFFUSE );


    return( Fl::run() );
}
