#include <iostream>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>               // for reference counting pointers
#include <Eigen/Eigen>                        // for vector maths
#include <pangolin/pangolin.h>                // for open GL state management
#include <SceneGraph/SceneGraph.h>            // for open GL scene graph
#include "GetPot"                             // for command line parsing
#include "CVarHelpers.h"                      // for parsing Eigen vars as CVars
#include <CVars/CVar.h>                       // for glconsole
#include <RPG/ModelGraph/Models.h>            // Include the ModelGraph
#include "RenderClass.h"
#include "PhysicsClass.h"

using namespace std;
using namespace CVarUtils;
using namespace pangolin;
using namespace SceneGraph;
using namespace boost;

#define USAGE    \
"USAGE: Robot -n <name> -d <media directory>\n"\
"      Options:\n"\
"      --RobotName, -n        Name of this Robot process.\n"\
"      --MediaDir, -d         Directory where game media (meshes, maps, models etc.) are stored.\n"


class App
{
    public:
        ///////////////////////////////////////////////////////////////////
        SceneGraph::GLShadowLight   m_light;
        SceneGraph::GLBox           m_ground;
        SceneGraph::GLGrid          m_grid;
        SceneGraph::GLSceneGraph&   m_rSceneGraph;
        std::string                 m_sRobotName;
        SceneGraph::GLMesh          m_RobotMesh;
        std::string                 m_sRobotMeshFilename;
        std::string                 m_sMediaDir; // path to media files, meshes, etc
        Model                       m_RobotModel;// focus of our example app
        Phys*                       m_Phys;
        Render                      m_Render;

        // TODO TODO
        // rpg::CameraDevice           cam;
        //rpg::CarDevice              car;


        ///////////////////////////////////////////////////////////////////
        App(
                SceneGraph::GLSceneGraph& glGraph,  //< Input: reference to glGraph
                const std::string& sRobotName,      //< Input: name of robot proxy
                const std::string& sMediaDir        //< Input: location of meshes, models, maps etc
                )
            : m_rSceneGraph( glGraph )/*, m_pSim(pSim)*/
        {
            // look in the given media dir for the robot models/meshes
            m_sMediaDir = sMediaDir;
            m_sRobotMeshFilename = m_sMediaDir+"/model.blend";
            m_sRobotName = sRobotName;
            BuildRobot();
            PrintRobotGraph( m_RobotModel );
            Eigen::Vector6d RobotPose;
            RobotPose << 0, 0, -10, 0, 0, 0;
//            BuildRenderGraph(m_RobotModel, RobotPose);
            m_Phys = new Phys;
            m_Phys->Init();
            AssociatePhysicsBodies( m_RobotModel, RobotPose );
            AssociatePhysicsJoints( m_RobotModel, RobotPose );
        }

        ///////////////////////////////////////////////////////////////////
        void BuildRobot( void )
        {
            //  First enter the root node to build off of
            BoxShape box = BoxShape(2,1,3);
            Body* pChassis = new Body( std::string("Chassis"), box, 1.0f );
            pChassis->SetPose( 0,0,0,0,0,0 );
            m_RobotModel.SetName("Robot");
            m_RobotModel.m_pParent = NULL;
            m_RobotModel.SetBase( pChassis ); // main body

            box = BoxShape(1,1,1.5);
            Body* pUpperRArm = new Body( "UpperRArm", box, 1 );
            pUpperRArm->SetPose( 3,0,-1.5, 0,0, 2);

            box = BoxShape(1,1,1.5);
            Body* pUpperLArm = new Body( "UpperLArm", box, 1 );
            pUpperLArm->SetPose( -3,0,-1.5, 0,0, -2 );

            box = BoxShape(1,1,1);
            Body* pHead = new Body( "Head", box, 1 );
            pHead->SetPose( 0,0,-5, 0,0, 0 );

            CylinderShape cylinder = CylinderShape(1, 2);
            Body* pRWheel = new Body("RWheel", cylinder, 1);
            pRWheel->SetPose(2.5, 0, 2.8, 0, M_PI / 2, 0);

            Body* pLWheel = new Body("LWheel", cylinder, 1);
            pLWheel->SetPose(-2.5, 0, 2.8, 0, M_PI / 2, 0);


            // create a joint connecting two bodies
            HingeJoint* pRHinge = new HingeJoint( "RArmJoint", pChassis, pUpperRArm, 2, 0, -2.8, 1,0,0 );
            HingeJoint* pLHinge = new HingeJoint( "LArmJoint", pChassis, pUpperLArm, -2, 0, -2.8, 1,0,0 );
            HingeJoint* pRAxle = new HingeJoint( "RAxleJoint", pChassis, pRWheel, 2.5, 0, 2.8, 1,0,0, 0, -1, 0 );
            HingeJoint* pLAxle = new HingeJoint( "LAxleJoint", pChassis, pLWheel, -2.5, 0, 2.8, 1,0,0, 0, -1, 0 );

            Eigen::Vector3d axis1;
            Eigen::Vector3d axis2;
            Eigen::Vector3d anchor;
            Eigen::Vector3d LowerLinearLimit;
            Eigen::Vector3d UpperLinearLimit;
            Eigen::Vector3d LowerAngleLimit;
            Eigen::Vector3d UpperAngleLimit;
            axis1 << 1, 0, 0;
            axis2 << 0, 0, -1;
            anchor << 0, 0, -13.5;
            LowerLinearLimit << 0, 0, -3;
            UpperLinearLimit << 0, 0, -5;
            LowerAngleLimit << 0, 0, 0;
            UpperAngleLimit << 1, 0, 0;
            Hinge2Joint* pNeck = new Hinge2Joint( "Neck", pChassis, pHead, axis1, axis2, anchor, 0.2, 0.2, LowerLinearLimit, UpperLinearLimit, LowerAngleLimit, UpperAngleLimit);
        }

        ///////////////////////////////////////////////////////////////////
        void AssociatePhysicsBodies( ModelNode& item, Eigen::Vector6d WorldPose )
        {
            if (dynamic_cast<Body*>(&item))
                m_Phys->RegisterObject( &item, item.GetName(), WorldPose);
            Eigen::Vector6d ChildWorldPose;
            for ( int count = 0; count < item.NumChildren(); count++ ) {
                ChildWorldPose = _T2Cart(_Cart2T(WorldPose)*(item.m_vChildren[count]->GetPoseMatrix()));
                AssociatePhysicsBodies(*(item.m_vChildren[count]), ChildWorldPose);
            }
        }

        ///////////////////////////////////////////////////////////////////
        void AssociatePhysicsJoints( ModelNode& item, Eigen::Vector6d WorldPose )
        {
            if (dynamic_cast<Joint*>(&item))
                m_Phys->RegisterObject( &item, item.GetName(), WorldPose);
            Eigen::Vector6d ChildWorldPose;
            for ( int count = 0; count < item.NumChildren(); count++ ) {
                ChildWorldPose = _T2Cart(_Cart2T(WorldPose)*(item.m_vChildren[count]->GetPoseMatrix()));
                AssociatePhysicsJoints(*(item.m_vChildren[count]), ChildWorldPose);
            }
        }

        ///////////////////////////////////////////////////////////////////
        void BuildRenderGraph( ModelNode& item, Eigen::Vector6d WorldPose )
        {
            m_Render.AddNode(&item, WorldPose);

            Eigen::Vector6d ChildWorldPose;
            for ( int count = 0; count < item.NumChildren(); count++ ) {
                ChildWorldPose = _T2Cart(_Cart2T(WorldPose)*(item.m_vChildren[count]->GetPoseMatrix()));
                BuildRenderGraph(*(item.m_vChildren[count]), ChildWorldPose);
            }
        }

        ///////////////////////////////////////////////////////////////////
        void PrintRobotGraph( ModelNode& mn )
        {
            if (mn.m_pParent == NULL)
                std::cout<<"I am "<<mn.GetName()<<" and I have "<<mn.NumChildren()<<" child(ren)."<<std::endl;
            else{
                std::cout<<"I am "<<mn.GetName()<<".  My parent is "<<mn.m_pParent->GetName()<<" and I have "<<mn.NumChildren()<<" child(ren)."<<std::endl;

                if (dynamic_cast<Body*>(&mn) != NULL){
                    Body* pBody = (Body*)(&mn);
                    if (dynamic_cast<BoxShape*>(pBody->m_RenderShape) != NULL)
                    {
                        std::cout<<"I am a Box"<<std::endl;
                    }
                    else if (dynamic_cast<CylinderShape*>(pBody->m_RenderShape) != NULL)
                    {
                        std::cout<<"I am a Cylinder"<<std::endl;
                    }
                    std::cout<<"I am a Body"<<std::endl;
                }else if (HingeJoint* test = dynamic_cast<HingeJoint*>(&mn)) {
                    std::cout<<"I am a HingeJoint"<<std::endl;
                } else if (HingeJoint* test = dynamic_cast<HingeJoint*>(&mn)) {
                    std::cout<<"I am a Hinge2Joint"<<std::endl;
                }
                else{
                    std::cout << "I don't seem to exist." << std::endl;
                }
            }

            for ( int count = 0; count < mn.NumChildren(); count++ ) {
                PrintRobotGraph(*(mn.m_vChildren[count]));
            }
        }

        ///////////////////////////////////////////////////////////////////
        void LeftKey()
        {
            Eigen::Matrix4d Tab = GLCart2T( 0, 0, 0, 0, 0, -0.1 );
            Eigen::Matrix4d Twa = GLCart2T( m_RobotMesh.GetPose() );
            Eigen::Matrix4d Twb = Twa*Tab;
            m_RobotMesh.SetPose( GLT2Cart(Twb) );
        }

        ///////////////////////////////////////////////////////////////////
        void RightKey()
        {
            Eigen::Matrix4d Tab = GLCart2T( 0, 0, 0, 0, 0, 0.1 );
            Eigen::Matrix4d Twa = GLCart2T( m_RobotMesh.GetPose() );
            Eigen::Matrix4d Twb = Twa*Tab;
            m_RobotMesh.SetPose( GLT2Cart(Twb) );
        }

        ///////////////////////////////////////////////////////////////////
        void ForwardKey()
        {
            Eigen::Matrix4d Tab = GLCart2T( 0.1, 0, 0, 0, 0, 0 );
            Eigen::Matrix4d Twa = GLCart2T( m_RobotMesh.GetPose() );
            Eigen::Matrix4d Twb = Twa*Tab;
            m_RobotMesh.SetPose( GLT2Cart(Twb) );
        }

        ///////////////////////////////////////////////////////////////////
        void ReverseKey()
        {
            Eigen::Matrix4d Tab = GLCart2T( -0.1, 0, 0, 0, 0, 0 );
            Eigen::Matrix4d Twa = GLCart2T( m_RobotMesh.GetPose() );
            Eigen::Matrix4d Twb = Twa*Tab;
            m_RobotMesh.SetPose( GLT2Cart(Twb) );
        }

        ///////////////////////////////////////////////////////////////////
        /// Re-allocate the simulator each time
        void InitReset()
        {
            m_rSceneGraph.Clear();
            m_Render.AddToScene( &m_rSceneGraph );

//            Eigen::Vector6d RobotPose;
//            RobotPose << 0, 0, -8, 0, 0, 0;
//            BuildRenderGraph(m_RobotModel, RobotPose);
//            if (m_Phys)
//            {
//                delete m_Phys;
//            }
//            m_Phys = new Phys;
//            m_Phys->Init();
//            AssociatePhysicsBodies( m_RobotModel, RobotPose );
//            AssociatePhysicsJoints( m_RobotModel, RobotPose );


            m_light.SetPosition( 10,10,-100 );
            m_rSceneGraph.AddChild( &m_light );

            m_grid.SetNumLines(20);
            m_grid.SetLineSpacing(1);
            m_rSceneGraph.AddChild(&m_grid);

            double dThickness = 1;
            m_ground.SetPose( 0,0, dThickness/2.0,0,0,0 );
            m_ground.SetExtent( 10,10, dThickness );

            BoxShape bs = BoxShape(10, 10, 1/2.0f);
            Body* ground = new Body("Ground", bs);
            ground->m_dMass = 0;
            ground->SetWPose( m_ground.GetPose4x4_po() );
            m_Phys->RegisterObject(ground, "Ground", m_ground.GetPose());

            Eigen::Vector6d InitPose;
            InitPose.setZero(6, 1);

            m_light.AddShadowReceiver( &m_ground );

            m_Render.UpdateScene();

        }

        void StepForward( void )
        {
            m_Phys->StepSimulation();
            m_Render.UpdateScene();
        }
};


///////////////////////////////////////////////////////////////////
int main( int argc, char** argv )
{
    // parse command line arguments
    GetPot cl( argc, argv );
    std::string sRobotName = cl.follow( "Bender", 2, "--RobotName", "-n" );
    std::string sMediaDir = cl.follow( "./", 2, "--MediaDir", "-d" ); // md specifies the media dir
//    if( argc != 5 ){
//        puts(USAGE);
//        return -1;
//    }

    // Create OpenGL window in single line thanks to GLUT
    pangolin::CreateGlutWindowAndBind("Main",640,480);
    SceneGraph::GLSceneGraph::ApplyPreferredGlSettings();
    glClearColor(0, 0, 0, 0);
    glewInit();

    // sinle application context holds everything
    SceneGraph::GLSceneGraph  glGraph;
    App app( glGraph, sRobotName, sMediaDir/*, pSim*/ ); // initialize exactly one RobotProxy
    app.InitReset(); // this will populate the scene graph with objects and
    // register these objects with the simulator.

    //////////////////////////////////////////////
    // <Pangolin boilerplate>
    const SceneGraph::AxisAlignedBoundingBox bbox = glGraph.ObjectAndChildrenBounds();
    const Eigen::Vector3d center = bbox.Center();
    const double size = bbox.Size().norm();
    const double far = 10*size;
    const double near = far / 1E3;

    // Define Camera Render Object (for view / scene browsing)
    pangolin::OpenGlRenderState stacks3d(
            pangolin::ProjectionMatrix(640,480,420,420,320,240,near,far),
            pangolin::ModelViewLookAt(center(0), center(1) + size, center(2) - size/4,
                center(0), center(1), center(2), pangolin::AxisNegZ) );

    // We define a new view which will reside within the container.
    pangolin::View view3d;

    // We set the views location on screen and add a handler which will
    // let user input update the model_view matrix (stacks3d) and feed through
    // to our scenegraph
    view3d.SetBounds( 0.0, 1.0, 0.0, 1.0, -640.0f/480.0f );
    view3d.SetHandler( new SceneGraph::HandlerSceneGraph( glGraph, stacks3d) );
    view3d.SetDrawFunction( SceneGraph::ActivateDrawFunctor( glGraph, stacks3d) );

    // Add our views as children to the base container.
    pangolin::DisplayBase().AddDisplay( view3d );

    // register a keyboard hook to trigger the reset method
    pangolin::RegisterKeyPressCallback( pangolin::PANGO_CTRL + 'r',
            boost::bind( &App::InitReset, &app ) );

    // simple asdw control
//    RegisterKeyPressCallback( 'a', bind( &Robot::LeftKey, &app ) );
//    RegisterKeyPressCallback( 'A', bind( &Robot::LeftKey, &app ) );

//    RegisterKeyPressCallback( 's', bind( &Robot::ReverseKey, &app ) );
//    RegisterKeyPressCallback( 'S', bind( &Robot::ReverseKey, &app ) );

//    RegisterKeyPressCallback( 'd', bind( &Robot::RightKey, &app ) );
//    RegisterKeyPressCallback( 'D', bind( &Robot::RightKey, &app ) );

//    RegisterKeyPressCallback( 'w', bind( &Robot::ForwardKey, &app ) );
//    RegisterKeyPressCallback( 'W', bind( &Robot::ForwardKey, &app ) );

    RegisterKeyPressCallback( ' ', bind( &App::StepForward, &app ) );
    RegisterKeyPressCallback( PANGO_CTRL + 'r', bind( &App::InitReset, &app ) );

    // </Pangolin boilerplate>
    //////////////////////////////////////////////

    // Default hooks for exiting (Esc) and fullscreen (tab).
    while( !pangolin::ShouldQuit() ) {

        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        GLColor(1, 1, 1, 1);

        DisplayBase().ActivateScissorAndClear();
        // 1) SENSE: Read from robot sensors

        // 2) PLAN:  do stuff

        // 3) ACT: Send commands to the robot

        // optionally, draw info for a human to look at

        app.m_Phys->DebugDrawWorld();
        app.m_Phys->StepSimulation();
//        app.m_Render.UpdateScene();

        // loop through robot graph and for each body update the associated GL object pose


        pangolin::FinishGlutFrame();
    }

    return 0;
}
