#ifndef _GL_LIGHT_H_
#define _GL_LIGHT_H_

#include <SceneGraph/GLSceneGraph.h>
#include <SceneGraph/GLAxis.h>

namespace SceneGraph
{

class GLLight : public GLObjectPrePostRender
{
public:
    GLLight(double x = 0, double y = 0, double z = 0)
        : GLObjectPrePostRender("Light")
    {
        m_nLigthId = (s_nNextLightId++);
        SetPerceptable(false);
        SetVisible(false);
        SetPosition(x,y,z);
    }

    ~GLLight()
    {
    }

    virtual void DrawCanonicalObject()
    {
        // TODO: Draw something that better resembles a light
        glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT);
        glDisable( GL_LIGHTING );
        GLAxis::DrawAxis(1.0);
        glPopAttrib();
    }

    void EnableLight()
    {
        glEnable( m_nLigthId );
        const Eigen::Matrix<double,6,1> pose = this->GetPose();
        const float pos[4] = {(float)pose(0),(float)pose(1),(float)pose(2), 1};
        glLightfv(m_nLigthId, GL_POSITION, pos );
    }

    void DisableLight()
    {
        glDisable(m_nLigthId);
    }

    virtual void PreRender(GLSceneGraph& /*scene*/) {
        EnableLight();
        GLfloat ambientLight [] = {0.2, 0.2, 0.2, 1.0};
        GLfloat diffuseLight [] = {0.4, 0.4, 0.4, 1.0};
        glLightfv(m_nLigthId, GL_AMBIENT, ambientLight);
        glLightfv(m_nLigthId, GL_DIFFUSE, diffuseLight);
    }

    virtual void PostRender(GLSceneGraph& /*scene*/) {
        DisableLight();
    }

protected:
    int m_nLigthId;
    static int s_nNextLightId;

};

}

#endif // _GL_LIGHT_H_
