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
        : GLObjectPrePostRender("Light"),
          m_ambient(0.2, 0.2, 0.2, 1.0),
          m_diffuse(0.8, 0.8, 0.8, 1.0)
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
//        glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT);
        glDisable( GL_LIGHTING );
        GLAxis::DrawAxis(1.0);
        glEnable( GL_LIGHTING );
    }

    void EnableLight()
    {
        glEnable( m_nLigthId );
        const Eigen::Matrix<double,6,1> pose = this->GetPose();
        const float pos[4] = {(float)pose(0),(float)pose(1),(float)pose(2), 1};
        glLightfv(m_nLigthId, GL_POSITION, pos );
        
        glLightfv(m_nLigthId, GL_AMBIENT, m_ambient.data());
        glLightfv(m_nLigthId, GL_DIFFUSE, m_diffuse.data());
    }

    void DisableLight()
    {
        glDisable(m_nLigthId);
    }

    virtual void PreRender(GLSceneGraph& /*scene*/) {
        EnableLight();
    }

    virtual void PostRender(GLSceneGraph& /*scene*/) {
        DisableLight();
    }

    virtual void SetAmbient(const Eigen::Vector4f& fAmbient){
        m_ambient = fAmbient;
    }

    virtual void SetDiffuse(const Eigen::Vector4f& fDiffuse){
        m_diffuse = fDiffuse;
    }

protected:
    Eigen::Vector4f m_ambient;
    Eigen::Vector4f m_diffuse;
    
    int m_nLigthId;
    static int s_nNextLightId;
};

}

#endif // _GL_LIGHT_H_
