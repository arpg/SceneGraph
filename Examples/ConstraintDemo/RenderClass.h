#ifndef RENDERCLASS_H
#define RENDERCLASS_H

#include <SceneGraph/SceneGraph.h>
#include <RobotGames/Shape.h>
#include <RobotGames/Models.h>
#include <RobotGames/SE3.h>
#include <map>

class SceneEntity {

};

class Render
{
public:
    Render()
    {
    }


    ~Render()
    {
        // write code to delete the Map
//        for ( int count = 0; count < objects.size(); count++ ){
//            delete objects[count];
//        }
    }

//    void AddShape( Shape *pShape, Eigen::Vector6d WorldPose )
//    {
//        if (dynamic_cast<BoxShape*>(pShape) != NULL)
//        {
//            BoxShape* pbShape = (BoxShape *) pShape;
//            SceneGraph::GLBox* new_box = new SceneGraph::GLBox();
//            new_box->SetExtent(pbShape->m_dDims[0], pbShape->m_dDims[1], pbShape->m_dDims[2]);
//            new_box->SetPose(WorldPose);
//            m_mSceneEntities.insert( pbShape , new_box);

//            std::cout<<"Rendering a Box at "<<WorldPose[0]<<", "<<WorldPose[1]<<", "<<WorldPose[2]<<std::endl;
//            std::cout<<"With dimensions "<< pbShape->m_dDims[0]<<", "<< pbShape->m_dDims[1]<<", "<< pbShape->m_dDims[2]<<" " <<std::endl;
//            std::cout<<"With dimensions "<< new_box->ObjectBounds().Min().transpose() << "-" << new_box->ObjectBounds().Max().transpose() << std::endl;
//        }
//    }

    void AddNode( ModelNode *pNode, Eigen::Vector6d WorldPose )
    {
        if (dynamic_cast<Body*>(pNode) != NULL){
            Body* pBody = (Body*)(pNode);
            if (dynamic_cast<BoxShape*>(pBody->m_RenderShape) != NULL)
            {
                BoxShape* pbShape = (BoxShape *) pBody->m_RenderShape;
                SceneGraph::GLBox* new_box = new SceneGraph::GLBox();
                new_box->SetExtent(pbShape->m_dDims[0]*2, pbShape->m_dDims[1]*2, pbShape->m_dDims[2]*2);
                new_box->SetPose(WorldPose);
                m_mSceneEntities[pNode] = new_box;

                pBody->SetWPose(_Cart2T(WorldPose));

                std::cout<<"Rendering a Box at "<<WorldPose[0]<<", "<<WorldPose[1]<<", "<<WorldPose[2]<<std::endl;
                std::cout<<"With dimensions "<< pbShape->m_dDims[0]*2<<", "<< pbShape->m_dDims[1]*2<<", "<< pbShape->m_dDims[2]*2<<" " <<std::endl;
//                std::cout<<"With dimensions "<< new_box->ObjectBounds().Min().transpose() << "-" << new_box->ObjectBounds().Max().transpose() << std::endl;
            }
            else if (dynamic_cast<CylinderShape*>(pBody->m_RenderShape) != NULL)
            {
                CylinderShape* pbShape = (CylinderShape *) pBody->m_RenderShape;
                SceneGraph::GLCylinder* new_cylinder = new SceneGraph::GLCylinder();
                new_cylinder->Init(pbShape->m_dRadius, pbShape->m_dRadius, pbShape->m_dThickness, 32, 1);


//                Eigen::Matrix4d Rx;
//                Rx << 1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1;

//                WorldPose = _T2Cart( *Rx );

                Eigen::Vector6d temp;
                temp << 0, 0, 0, M_PI / 2, 0, 0;
                new_cylinder->SetPose((Eigen::Matrix4d)(_Cart2T(WorldPose)*_Cart2T(temp)));
                m_mSceneEntities[pNode] = new_cylinder;

                pBody->SetWPose(_Cart2T(WorldPose));

                std::cout<<"Rendering a Cylinder at "<<WorldPose[0]<<", "<<WorldPose[1]<<", "<<WorldPose[2]<<", "<<WorldPose[3]<<", "<<WorldPose[4]<<", "<<WorldPose[5]<<std::endl;
                std::cout<<"With dimensions "<< pbShape->m_dRadius<<", "<< pbShape->m_dThickness<<std::endl;
//                std::cout<<"With dimensions "<< new_box->ObjectBounds().Min().transpose() << "-" << new_box->ObjectBounds().Max().transpose() << std::endl;
            }

        }
    }


    void AddToScene( SceneGraph::GLSceneGraph *glGraph )
    {
//        for ( int count = 0; count < objects.size(); count++ ){
//            glGraph->AddChild( objects[count] );
//        }
        std::map<ModelNode*, SceneGraph::GLObject* >::iterator it;
        for(it=m_mSceneEntities.begin(); it != m_mSceneEntities.end(); it++) {
            SceneGraph::GLObject* p = it->second;
            glGraph->AddChild( p );
        }

    }

    void UpdateScene( void )
    {
        std::map<ModelNode*, SceneGraph::GLObject*>::iterator it;
        for(it=m_mSceneEntities.begin(); it != m_mSceneEntities.end(); it++) {
            ModelNode* mn = it->first;
            SceneGraph::GLObject* p = it->second;
            p->SetPose( mn->GetWPose() );
        }
    }

    //std::vector<SceneGraph::GLObject*> objects;
    std::map<ModelNode*, SceneGraph::GLObject*> m_mSceneEntities;

};

#endif // RENDERCLASS_H
