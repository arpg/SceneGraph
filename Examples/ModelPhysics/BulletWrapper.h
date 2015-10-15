#ifndef __BULLET_WRAPPER_
#define __BULLET_WRAPPER_

#include <memory>
#include <pangolin/pangolin.h>
#include <SceneGraph/SceneGraph.h>

// Suppress Bullet warnings in GCC and Clang
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wextra"
#include <bullet/btBulletDynamicsCommon.h>
#pragma GCC diagnostic pop

inline Eigen::Matrix<double,4,4> toEigen(const btTransform& T)
{
    Eigen::Matrix<btScalar,4,4> eT;
    T.getOpenGLMatrix(eT.data());
    return eT.cast<double>();
}

inline btTransform toBullet(const Eigen::Matrix<double,4,4>& T)
{
    btTransform bT;
    Eigen::Matrix<btScalar,4,4> eT = T.cast<btScalar>();
    bT.setFromOpenGLMatrix(eT.data());
    return bT;
}

inline btVector3 toBulletVec3(const Eigen::Vector3d& v)
{
    btVector3 bv;
    bv.setX(v(0));
    bv.setY(v(1));
    bv.setZ(v(2));
    return bv;
}

class SceneGraphMotionState : public btMotionState {
    public:
        SceneGraphMotionState(SceneGraph::GLObject& obj)
            : object(obj)
        {
        }

        virtual void getWorldTransform(btTransform &worldTrans) const {
            worldTrans = toBullet(object.GetPose4x4_po());
        }

        virtual void setWorldTransform(const btTransform &worldTrans) {
            object.SetPose(toEigen(worldTrans));
        }

    protected:
        SceneGraph::GLObject& object;
};

typedef  std::shared_ptr<btCollisionShape>            CollisionShapePtr;
typedef  std::shared_ptr<SceneGraphMotionState>       MotionStatePtr;
typedef  std::shared_ptr<btRigidBody>                 RigidBodyPtr;

class Entity
{
    public:
        Entity()
        {

        }

        Entity(
                CollisionShapePtr  pShape, //< Input:
                MotionStatePtr  pMotionState, //< Input:
                RigidBodyPtr pRigidBody //< Input:
                )
        {
            m_pShape       = pShape;
            m_pMotionState = pMotionState;
            m_pRigidBody   = pRigidBody;
        }

//    private:
        CollisionShapePtr       m_pShape;
        MotionStatePtr          m_pMotionState;
        RigidBodyPtr            m_pRigidBody;
};

// The goal here is to wrap bullet so our objects don't need to know about
// bullet... also so we can potentially swap bullet out for some other
// simulator down the road.
class Sim 
{

    public:

        ///////////////////////////////////////////////////////////////////
        Sim()
        {
            m_dTimeStep = 1.0/30.0;
            m_dGravity = 9.8;
            m_nMaxSubSteps = 10; // bullet -- for stepSimulation

        }

        void Init( 
                double dGravity = 9.8,       //< Input:
                double dTimeStep = 1.0/30.0, //< Input: 
                double nMaxSubSteps = 10     //< Input: for stepSimulation
                )
        {

            m_dTimeStep    = dTimeStep;
            m_dGravity     = dGravity;
            m_nMaxSubSteps = nMaxSubSteps;


            // Physics stuff see http://bulletphysics.org/mediawiki-1.5.8/index.php/Hello_World
            m_pDispatcher 
                = std::shared_ptr<btCollisionDispatcher>( new btCollisionDispatcher(&m_CollisionConfiguration) );
            // Build the broadphase (approximate collision detection)
            m_pBroadphase 
                = std::shared_ptr<btDbvtBroadphase>( new btDbvtBroadphase );
            m_pSolver
                = std::shared_ptr<btSequentialImpulseConstraintSolver>( new btSequentialImpulseConstraintSolver );

            /// the main machine
            m_pDynamicsWorld = std::shared_ptr<btDiscreteDynamicsWorld>(new btDiscreteDynamicsWorld(
                        m_pDispatcher.get(),
                        m_pBroadphase.get(),
                        m_pSolver.get(),
                        &m_CollisionConfiguration
                        ) );
            m_pDynamicsWorld->setGravity( btVector3(0,0,m_dGravity) );

        }
        /*

           this kind of thing should be automatic with shared_ptr's -- TOOD: make sure of this

           m_pDynamicsWorld->removeRigidBody( cubeRigidBody );
           delete cubeRigidBody->getMotionState();
           delete cubeRigidBody;

        //    m_pDynamicsWorld->removeRigidBody(groundRigidBody);
        //    delete groundRigidBody->getMotionState();
        //    delete groundRigidBody;
        //    delete groundShape;

        delete cubeShape;
        delete m_pDynamicsWorld;
         */


        ///////////////////////////////////////////////////////////////////
        int RegisterObject( 
                SceneGraph::GLObject* pObj, 
                double dMass = 0, 
                double dDefaultRestitution = 0
                )
        {

            btVector3 bounds = toBulletVec3(pObj->ObjectBounds().HalfSizeFromOrigin());
            CollisionShapePtr pShape( new btBoxShape(bounds) );
            // use our SceneGraphMotionState class, which inherits from btDefaultMotionState
            MotionStatePtr pMotionState( new SceneGraphMotionState(*pObj) );

            btAssert((!pShape || pShape->getShapeType() != INVALID_SHAPE_PROXYTYPE));
            //rigidbody is dynamic if and only if mass is non zero, otherwise static
            bool isDynamic = ( dMass != 0.f );

            btVector3 localInertia( 0, 0, 0 );
            if( isDynamic ){
                pShape->calculateLocalInertia( dMass, localInertia );
            }

            btRigidBody::btRigidBodyConstructionInfo  cInfo( dMass, pMotionState.get(), pShape.get(), localInertia );
            RigidBodyPtr  body( new btRigidBody(cInfo) );

            double dDefaultContactProcessingThreshold = 0.001;
            body->setContactProcessingThreshold( dDefaultContactProcessingThreshold );
            body->setRestitution( dDefaultRestitution );

            m_pDynamicsWorld->addRigidBody( body.get() );

            // save this object somewhere (to keep it's reference count above 0)
            std::shared_ptr<Entity> pEntity( new Entity ); 
            pEntity->m_pRigidBody = body;
            pEntity->m_pShape = pShape;
            pEntity->m_pMotionState = pMotionState;

            int id = m_mEntities.size();
            m_mEntities[id] = pEntity;
            return id;
        }

        ///////////////////////////////////////////////////////////////////
        void StepSimulation()
        {
            m_pDynamicsWorld->stepSimulation( m_dTimeStep,  (float)m_nMaxSubSteps );
        }


        std::map<int,std::shared_ptr<Entity> > m_mEntities;

    private:

        /*
        ///////////////////////////////////////////////////////////////////
        void _CreateBulletRigidBody(
                float mass,
                btMotionState* pMotionState,
                btCollisionShape* shape,
                double dDefaultRestitution = 0
                )
        {
            btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));
            //rigidbody is dynamic if and only if mass is non zero, otherwise static
            bool isDynamic = ( mass != 0.f );

            btVector3 localInertia( 0, 0, 0 );
            if( isDynamic ){
                shape->calculateLocalInertia( mass, localInertia );
            }

            btRigidBody::btRigidBodyConstructionInfo  cInfo( mass, pMotionState, shape, localInertia );
            RigidBodyPtr  body( new btRigidBody(cInfo) );

            double dDefaultContactProcessingThreshold = 0.001;
            body->setContactProcessingThreshold( dDefaultContactProcessingThreshold );
            body->setRestitution( dDefaultRestitution );

            m_pDynamicsWorld->addRigidBody( body.get() );
        }
        */


        ///////////////////////////////////////////////////////////////////
        btDefaultCollisionConfiguration                        m_CollisionConfiguration;
        std::shared_ptr<btCollisionDispatcher>               m_pDispatcher;
        std::shared_ptr<btDbvtBroadphase>                    m_pBroadphase;
        std::shared_ptr<btSequentialImpulseConstraintSolver> m_pSolver;
        std::shared_ptr<btDiscreteDynamicsWorld>             m_pDynamicsWorld;
        double                                                 m_dTimeStep;
        double                                                 m_dGravity;
        int                                                    m_nMaxSubSteps;
};





#endif
