#include "GLBulletDebugDrawer.h"

GLBulletDebugDrawer::GLBulletDebugDrawer()
{
}


void GLBulletDebugDrawer::Init(BulletCarModel* pModel)
{
    m_pModel = pModel;
}

void GLBulletDebugDrawer::DrawCanonicalObject()
{
    m_pModel->DebugDrawWorld(0);
}
