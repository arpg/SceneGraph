#include <iostream>
#include <functional>
#include <Eigen/Eigen>
#include <unistd.h>

#include <pangolin/pangolin.h>
#include <SceneGraph/SceneGraph.h>
#include "BulletWrapper.h"

using namespace std;

int main( int /*argc*/, char** /*argv*/ )
{
    // Create OpenGL window in single line thanks to GLUT
    pangolin::CreateWindowAndBind("Main",640,480);
    SceneGraph::GLSceneGraph::ApplyPreferredGlSettings();
    glewInit();

    // Scenegraph to hold GLObjects and relative transformations
    SceneGraph::GLSceneGraph glGraph;

    SceneGraph::GLLight light(10,10,-100);
    glGraph.AddChild(&light);

    SceneGraph::GLGrid grid(10,1,true);
    glGraph.AddChild(&grid);

    SceneGraph::GLCube cube;
    cube.SetPose(0,0,20,M_PI_4,M_PI_4,0.1);
    glGraph.AddChild(&cube);

    const SceneGraph::AxisAlignedBoundingBox bbox = glGraph.ObjectAndChildrenBounds();
    const Eigen::Vector3d center = bbox.Center();
    const double size = bbox.Size().norm();
    const double far = 10*size;
    const double near = far / 1E3;

    // Define Camera Render Object (for view / scene browsing)
    pangolin::OpenGlRenderState stacks3d(
        pangolin::ProjectionMatrix(640,480,420,420,320,240,near,far),
        pangolin::ModelViewLookAt(center(0), center(1) + size, center(2) + size/4, center(0), center(1), center(2), pangolin::AxisZ)
    );

    // We define a new view which will reside within the container.
    pangolin::View view3d;

    // We set the views location on screen and add a handler which will
    // let user input update the model_view matrix (stacks3d) and feed through
    // to our scenegraph
    view3d.SetBounds(0.0, 1.0, 0.0, 1.0, -640.0f/480.0f)
          .SetHandler(new SceneGraph::HandlerSceneGraph(glGraph,stacks3d))
          .SetDrawFunction(SceneGraph::ActivateDrawFunctor(glGraph, stacks3d));

    // Add our views as children to the base container.
    pangolin::DisplayBase().AddDisplay(view3d);

    // Physics stuff
    // Build the broadphase (approximate collision detection)
    btDbvtBroadphase broadphase;
    btDefaultCollisionConfiguration collisionConfiguration;
    btCollisionDispatcher dispatcher(&collisionConfiguration);
    btSequentialImpulseConstraintSolver solver;
    btDiscreteDynamicsWorld dynamicsWorld(&dispatcher,&broadphase,&solver,&collisionConfiguration);
    dynamicsWorld.setGravity(btVector3(0,0,-10));

    btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,0,1),0);
    btCollisionShape* fallShape = new btBoxShape(toBulletVec3( cube.ObjectBounds().HalfSizeFromOrigin() ) );

    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,0,0)));
    btRigidBody::btRigidBodyConstructionInfo
            groundRigidBodyCI(0,groundMotionState,groundShape,btVector3(0,0,0));
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
    groundRigidBody->setRestitution(0.1);
    dynamicsWorld.addRigidBody(groundRigidBody);


    SceneGraphMotionState* fallMotionState = new SceneGraphMotionState(cube);
    btScalar mass = 1;
    btVector3 fallInertia(0,0,0);
    fallShape->calculateLocalInertia(mass,fallInertia);
    btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,fallMotionState,fallShape,fallInertia);
    btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
    fallRigidBody->setRestitution(5);
    dynamicsWorld.addRigidBody(fallRigidBody);

    // Default hooks for exiting (Esc) and fullscreen (tab).
    while( !pangolin::ShouldQuit() )
    {
        // Clear whole screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Swap frames and Process Events
        pangolin::FinishFrame();

        // Pause for 1/60th of a second.
        usleep(1E6 / 60);
        dynamicsWorld.stepSimulation(1/60.f,10);
    }

    dynamicsWorld.removeRigidBody(fallRigidBody);
    delete fallRigidBody->getMotionState();
    delete fallRigidBody;

    dynamicsWorld.removeRigidBody(groundRigidBody);
    delete groundRigidBody->getMotionState();
    delete groundRigidBody;


    delete fallShape;

    delete groundShape;

    return 0;
}
