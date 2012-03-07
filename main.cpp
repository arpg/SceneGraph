#include "GLWindow.h"
#include "GLGrid.h"
#include "GLText.h"
#include "GLLineStrip.h"
#include "GLCar.h"
#include "GLWayPoint.h"
#include "GLRamp.h"
#include "GLMesh.h"

#include <GetPot>

#include <CarPlanner.h>

#include <CVars/CVarVectorIO.h>

#include "Node.h"
#include "Messages.pb.h"


using namespace CVarUtils;

////////////////////////////////////////////////////////////////////////////
// GL Object List
std::vector<GLLineStrip>		gGLLineSegments;
std::vector<GLWayPoint>			gGLWayPoints;
GLGrid							gGLGrid;
GLText							gGLLoc;
GLText							gGLLocType;
GLCar							gGLCar;
GLCar							gGLDesCar;
GLRamp							gGLRamp1;

CarPlanner		gPlanner;  // Car planner for trajectory plotting

rpg::Node			gNode(5555);
std::string         g_sMochaPubSubUri;
std::string         g_sMochaRpcUri;


std::vector<Eigen::Vector5d*>	gWayPoints;
std::vector<int>&				gPath = CVarUtils::CreateCVar( "path", std::vector<int>(), "Path vector.");

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
	gNode.Call( g_sMochaRpcUri, "Waypoints", ReqMsg, ReqMsg);
	FLConsoleInstance* pConsole = GetConsole();
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
	gNode.Call( g_sMochaRpcUri, "Path", ReqMsg, ReqMsg);
	FLConsoleInstance* pConsole = GetConsole();
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
	gNode.Call( g_sMochaRpcUri, "Start", ReqMsg, ReqMsg);
	FLConsoleInstance* pConsole = GetConsole();
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
	gNode.Call( g_sMochaRpcUri, "Stop", ReqMsg, ReqMsg);
	FLConsoleInstance* pConsole = GetConsole();
	if( RepMsg.val() ) {
		pConsole->Printf("Mochaccino stopped...");
	} else {
		pConsole->PrintError("Error stopping Mochaccino.");
	}
	return true;
}





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
void Update( void* pUserData )
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


    Fl::repeat_timeout( 1.0/30.0, Update, NULL );
}



////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

int main( int argc, char** argv )
{
    GetPot cl(argc,argv);

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
	pWin->RegisterObject(&gGLGrid);
	pWin->RegisterObject(&gGLLoc);
	pWin->RegisterObject(&gGLLocType);
	pWin->RegisterObject(&gGLCar);
	pWin->RegisterObject(&gGLDesCar);
//	pWin->RegisterObject(&gGLRamp1);

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
	gGLCar.SetColor(GLColor(0,0,1));
	gGLDesCar.InitReset();
	gGLDesCar.SetScale(0.4);
	gGLDesCar.SetColor(GLColor(0,1,0));
	gGLRamp1.InitReset();


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

    // set timeoutfunction:
    Fl::add_timeout( g_dFPS, Update );

    return( Fl::run() );
}
