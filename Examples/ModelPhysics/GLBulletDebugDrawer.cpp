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
    //glPushAttrib(GL_ENABLE_BIT);
    //glDisable(GL_LIGHTING);
    //glColor3f(1.0f,1.0f,1.0f);

    m_pModel->DebugDrawWorld(0);

    //glPopAttrib();
}
