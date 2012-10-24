#ifndef _GL_LIGHT_H_
#define _GL_LIGHT_H_

#include <SceneGraph/GLObject.h>
#include <SceneGraph/GLAxis.h>

namespace SceneGraph
{

class GLLight : public GLObject
{
public:
    GLLight()
        : GLObject("Light")
    {
        SetPerceptable(false);
        SetVisible(false);
    }

    ~GLLight()
    {
    }

    void DrawCanonicalObject()
    {
        // TODO: Draw something that better resembles a light
        glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT);
        glDisable( GL_LIGHTING );
        GLAxis::DrawAxis(1.0);
        glPopAttrib();

   }

    void ApplyAsGlLight(const GLenum gl_light)
    {
        glEnable( gl_light );
        const Eigen::Matrix<double,6,1> pose = this->GetPose();
        const float pos[4] = {(float)pose(0),(float)pose(1),(float)pose(2), 1};
        glLightfv(gl_light, GL_POSITION, pos );
    }

protected:

};

}

#endif // _GL_LIGHT_H_
