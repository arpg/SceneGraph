#ifndef GLBULLETDEBUGDRAWER_H
#define GLBULLETDEBUGDRAWER_H

#include "LocalPlanner.h"
#include "pangolin/pangolin.h"
#include "SceneGraph/SceneGraph.h"


using namespace SceneGraph;

class GLBulletDebugDrawer : public GLObject
{
public:
    GLBulletDebugDrawer();
    void Init(BulletCarModel* pModel);
    virtual void DrawCanonicalObject();

private:
    BulletCarModel* m_pModel;
};

#endif // GLBULLETDEBUGDRAWER_H
