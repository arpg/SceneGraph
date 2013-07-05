#pragma once

#include <pangolin/gl_es_compat.h>
#include <Eigen/Eigen>
#include <stack>

#define GL_ENABLE_BIT 0
#define GL_LUMINANCE8 GL_LUMINANCE 

inline void glMultMatrixd( double *m )
{
    glMultMatrixf( ((Eigen::Matrix4f) (Eigen::Map<Eigen::Matrix4d>(m).cast<float>())).data() );
}

inline void glScaled( double x, double y, double z)
{
    glScalef( (GLfloat)x, (GLfloat)y, (GLfloat)z);
}
