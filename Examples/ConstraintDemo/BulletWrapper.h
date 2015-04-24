#ifndef __BULLET_WRAPPER_
#define __BULLET_WRAPPER_

#include <pangolin/pangolin.h>
#include <SceneGraph/SceneGraph.h>
#include <bullet/btBulletDynamicsCommon.h>
#include "GlutDemoApplication.h"

using namespace std;

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
                string sName,
                CollisionShapePtr  pShape, //< Input:
                MotionStatePtr  pMotionState, //< Input:
                RigidBodyPtr pRigidBody //< Input:
                )
        {
            //m_sName        = sName;
            m_pShape       = pShape;
            m_pMotionState = pMotionState;
            m_pRigidBody   = pRigidBody;
        }

//    private:
        //string                  m_sName;
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

        RigidBodyPtr m_carBody;
        ///////////////////////////////////////////////////////////////////
        Sim()
        {
            m_dTimeStep = 1.0/30.0;
            m_dGravity = 9.8;
            m_nMaxSubSteps = 10; // bullet -- for stepSimulation

        }

        void    Init(
                double dGravity = 9.8,       //< Input:
                double dTimeStep = 1.0/60.0, //< Input:
                double nMaxSubSteps = 1     //< Input: for stepSimulation
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

        /* GenerateDynamicConvexHull is used for concaveDynamicShape*/

        void GenerateDynamicConcaveHull( const struct aiScene *pAIScene,
                                  const struct aiNode *pAINode,
                                  const aiMatrix4x4 parentTransform,
                                  const float flScale,
                                  btTriangleMesh &triMesh,
                                  btVector3& dMin,
                                  btVector3& dMax )
        {
            aiMesh *pAIMesh;
            aiFace *pAIFace;
            for ( int x = 0; x < pAINode->mNumMeshes; x++ )
            {
                pAIMesh = pAIScene->mMeshes[pAINode->mMeshes[x]];
                for ( int y = 0; y < pAIMesh->mNumFaces; y++ )
                {
                    pAIFace = &pAIMesh->mFaces[y];

                    if ( pAIFace->mNumIndices != 3 )
                    {
                        /*if ( bEnableDebugging )
                        {
                            std::cout << "WARNING: A non-triangle face has been detected on this mesh, which is currently not supported." << std::endl;
                            std::cout << "         As such, this face will not be used to generate a collision shape for this mesh." << std::endl;
                            std::cout << "         This could have disastrous consequences. Consider using a different mesh!" << std::endl;
                        }*/
                        continue;
                    }
                    aiVector3D v1 = parentTransform * pAIMesh->mVertices[pAIFace->mIndices[0]];
                    aiVector3D v2 = parentTransform * pAIMesh->mVertices[pAIFace->mIndices[1]];
                    aiVector3D v3 = parentTransform * pAIMesh->mVertices[pAIFace->mIndices[2]];

                    btVector3 btV1(v1.x,v1.y,v1.z);
                    btVector3 btV2(v2.x,v2.y,v2.z);
                    btVector3 btV3(v3.x,v3.y,v3.z);

                    triMesh.addTriangle(btV1,btV2,btV3);
//                    hullMesh->addPoint(btVector3(v1.x*flScale, v1.y*flScale, v1.z*flScale));
//                    hullMesh->addPoint(btVector3(v1.x*flScale, v1.y*flScale, v1.z*flScale));
//                    hullMesh->addPoint(btVector3(v1.x*flScale, v1.y*flScale, v1.z*flScale));

                    dMin[0] = std::min((float)dMin[0],std::min(v1.x,std::min(v2.x, v3.x)));
                    dMax[0] = std::max((float)dMax[0],std::max(v1.x,std::max(v2.x, v3.x)));
                    dMin[1] = std::min((float)dMin[1],std::min(v1.y,std::min(v2.y, v3.y)));
                    dMax[1] = std::max((float)dMax[1],std::max(v1.y,std::max(v2.y, v3.y)));
                    dMin[2] = std::min((float)dMin[2],std::min(v1.z,std::min(v2.z, v3.z)));
                    dMax[2] = std::max((float)dMax[2],std::max(v1.z,std::max(v2.z, v3.z)));
                }
            }
            for ( int x = 0; x < pAINode->mNumChildren; x++ ){
                GenerateDynamicConcaveHull( pAIScene,
                                     pAINode->mChildren[x],
                                     parentTransform*pAINode->mChildren[x]->mTransformation,
                                     flScale,
                                     triMesh,
                                     dMin,
                                     dMax );
            }
        }

        void RegisterConcaveMesh(
                SceneGraph::GLMesh* pMesh,
                string sName,
                double dMass = 0,
                double dDefaultRestitution = 0
                )
        {
            btTriangleMesh *triMesh = new btTriangleMesh();
            //btConvexHullShape *convexHullShape = new btConvexHullShape();

            btVector3 dMax (pMesh->ObjectBounds().Max()[0],
                            pMesh->ObjectBounds().Max()[1],
                            pMesh->ObjectBounds().Max()[2]);

            btVector3 dMin (pMesh->ObjectBounds().Min()[0],
                            pMesh->ObjectBounds().Min()[1],
                            pMesh->ObjectBounds().Min()[2]);

            GenerateDynamicConcaveHull(pMesh->GetScene(),
                                pMesh->GetScene()->mRootNode,
                                pMesh->GetScene()->mRootNode->mTransformation,
                                pMesh->GetScale()[0],
                                *triMesh,
                                dMax,
                                dMin);

            btCollisionShape *landshape;
            landshape = new btBvhTriangleMeshShape(triMesh, true);

            //btVector3 bounds = toBulletVec3(pObj->ObjectBounds().HalfSizeFromOrigin());
            CollisionShapePtr pShape( landshape );
            // use our SceneGraphMotionState class, which inherits from btDefaultMotionState
            MotionStatePtr pMotionState( new SceneGraphMotionState(*pMesh) );

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
            //pEntity->m_sName = sName;

            m_mEntities[sName] = pEntity;
            return;
        }


        /* GenerateDynamicConvexHull is used for ConvexDynamicShape*/

        void GenerateDynamicConvexHull( const struct aiScene *pAIScene,
                                  const struct aiNode *pAINode,
                                  const aiMatrix4x4 parentTransform,
                                  const float flScale,
                                  btConvexHullShape &hullMesh,
                                  btVector3& dMin,
                                  btVector3& dMax )
        {
            aiMesh *pAIMesh;
            aiFace *pAIFace;
            for ( int x = 0; x < pAINode->mNumMeshes; x++ )
            {
                pAIMesh = pAIScene->mMeshes[pAINode->mMeshes[x]];
                for ( int y = 0; y < pAIMesh->mNumFaces; y++ )
                {
                    pAIFace = &pAIMesh->mFaces[y];
                    if ( pAIFace->mNumIndices != 3 )
                    {
                        /*if ( bEnableDebugging )
                        {
                            std::cout << "WARNING: A non-triangle face has been detected on this mesh, which is currently not supported." << std::endl;
                            std::cout << "         As such, this face will not be used to generate a collision shape for this mesh." << std::endl;
                            std::cout << "         This could have disastrous consequences. Consider using a different mesh!" << std::endl;
                        }*/
                        continue;
                    }
                    aiVector3D v1 = parentTransform * pAIMesh->mVertices[pAIFace->mIndices[0]];
                    aiVector3D v2 = parentTransform * pAIMesh->mVertices[pAIFace->mIndices[1]];
                    aiVector3D v3 = parentTransform * pAIMesh->mVertices[pAIFace->mIndices[2]];
                    hullMesh.addPoint(btVector3(v1.x*flScale, v1.y*flScale, v1.z*flScale));
                    hullMesh.addPoint(btVector3(v1.x*flScale, v1.y*flScale, v1.z*flScale));
                    hullMesh.addPoint(btVector3(v1.x*flScale, v1.y*flScale, v1.z*flScale));
                    dMin[0] = std::min((float)dMin[0],std::min(v1.x,std::min(v2.x, v3.x)));
                    dMax[0] = std::max((float)dMax[0],std::max(v1.x,std::max(v2.x, v3.x)));
                    dMin[1] = std::min((float)dMin[1],std::min(v1.y,std::min(v2.y, v3.y)));
                    dMax[1] = std::max((float)dMax[1],std::max(v1.y,std::max(v2.y, v3.y)));
                    dMin[2] = std::min((float)dMin[2],std::min(v1.z,std::min(v2.z, v3.z)));
                    dMax[2] = std::max((float)dMax[2],std::max(v1.z,std::max(v2.z, v3.z)));
                }
            }
            for ( int x = 0; x < pAINode->mNumChildren; x++ ){
                GenerateDynamicConvexHull( pAIScene,
                                     pAINode->mChildren[x],
                                     parentTransform*pAINode->mChildren[x]->mTransformation,
                                     flScale,
                                     hullMesh,
                                     dMin,
                                     dMax );
            }
        }

        void RegisterConvexMesh(
                SceneGraph::GLMesh* pMesh,
                string sName,
                double dMass = 0,
                double dDefaultRestitution = 0
                )
        {
            btConvexHullShape *convexHullShape = new btConvexHullShape();

            btVector3 dMax (pMesh->ObjectAndChildrenBounds().Max()[0],
                            pMesh->ObjectAndChildrenBounds().Max()[1],
                            pMesh->ObjectAndChildrenBounds().Max()[2]);

            btVector3 dMin (pMesh->ObjectAndChildrenBounds().Min()[0],
                            pMesh->ObjectAndChildrenBounds().Min()[1],
                            pMesh->ObjectAndChildrenBounds().Min()[2]);

            GenerateDynamicConvexHull(pMesh->GetScene(),
                                pMesh->GetScene()->mRootNode,
                                pMesh->GetScene()->mRootNode->mTransformation,
                                pMesh->GetScale()[0],
                                *convexHullShape,
                                dMax,
                                dMin);


            //btVector3 bounds = toBulletVec3(pObj->ObjectBounds().HalfSizeFromOrigin());
            CollisionShapePtr pShape( convexHullShape );
            // use our SceneGraphMotionState class, which inherits from btDefaultMotionState
            MotionStatePtr pMotionState( new SceneGraphMotionState(*pMesh) );

            btAssert((!pShape || pShape->getShapeType() != INVALID_SHAPE_PROXYTYPE));
            //rigidbody is dynamic if and only if mass is non zero, otherwise static
            bool isDynamic = ( dMass != 0.f );

            btVector3 localInertia( 0, 0, 0 );
            if( isDynamic ){
                pShape->calculateLocalInertia( dMass, localInertia );
            }

            btRigidBody::btRigidBodyConstructionInfo  cInfo( dMass, pMotionState.get(), pShape.get(), localInertia );

            RigidBodyPtr body( new btRigidBody(cInfo) );

            double dDefaultContactProcessingThreshold = 0.001;
            body->setContactProcessingThreshold( dDefaultContactProcessingThreshold );
            body->setRestitution( dDefaultRestitution );

            m_pDynamicsWorld->addRigidBody( body.get() );

            // save this object somewhere (to keep it's reference count above 0)
            std::shared_ptr<Entity> pEntity( new Entity );
            pEntity->m_pRigidBody = body;
            pEntity->m_pShape = pShape;
            pEntity->m_pMotionState = pMotionState;
            //pEntity->m_sName = sName;

            m_mEntities[sName] = pEntity;
            return;
        }

        ///////////////////////////////////////////////////////////////////
        void RegisterObject(
                SceneGraph::GLObject* pObj, 
                string sName,
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

            //int id = m_mEntities.size();
            m_mEntities[sName] = pEntity;
            return;
        }

        ///////////////////////////////////////////////////////////////////
        void StepSimulation()
        {
            m_pDynamicsWorld->stepSimulation( m_dTimeStep,  m_nMaxSubSteps );
        }

        Entity getEntity(string name){
           return  *m_mEntities.find(name)->second;
        }

        std::map<string,std::shared_ptr<Entity> > m_mEntities;

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
