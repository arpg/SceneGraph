#pragma once

#include <pangolin/glinclude.h>
#include <Eigen/Eigen>

#define GL_ENABLE_BIT 0
#define GL_LUMINANCE8 GL_LUMINANCE 

inline void glPushAttrib(GLbitfield mask)
{
    // TODO: Something!
}

inline void glPopAttrib()
{
    // TODO: Something!
}

inline void glMultMatrixd ( double *m )
{
    glMultMatrixf( ((Eigen::Matrix4f) (Eigen::Map<Eigen::Matrix4d>(m).cast<float>())).data() );
}
