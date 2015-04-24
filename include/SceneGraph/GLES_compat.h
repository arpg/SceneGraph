#pragma once

#include <pangolin/gl_es_compat.h>
#include <Eigen/Eigen>

#ifdef HAVE_GLUES
#include <glues/glues.h>
#endif

#define GL_ENABLE_BIT 0
#define GL_LUMINANCE8 GL_LUMINANCE

#ifndef GL_DEPTH_COMPONENT
#   define GL_DEPTH_COMPONENT GL_DEPTH_COMPONENT24
#endif

#define glActiveTextureARB glActiveTexture
#define GL_TEXTURE1_ARB GL_TEXTURE1
#define GL_TEXTURE0_ARB GL_TEXTURE0

inline void glMultMatrixd(const double *m )
{
  glMultMatrixf(static_cast<Eigen::Matrix4f>(
      Eigen::Map<const Eigen::Matrix4d>(m).cast<float>()).data());
}

inline void glScaled( double x, double y, double z)
{
    glScalef( (GLfloat)x, (GLfloat)y, (GLfloat)z);
}
