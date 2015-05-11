//  Copyright 2014 ARPG

#ifndef SCENEGRAPH_GLCYLINDER_H_
#define SCENEGRAPH_GLCYLINDER_H_

#include <math.h>
#include <SceneGraph/GLObject.h>

namespace SceneGraph {

class GLCylinder : public GLObject {
 public:
  explicit GLCylinder(GLuint texId = 0);
  ~GLCylinder();
  void Init(
      double base_radius,
      double top_radius,
      double height,
      int    slices,
      int    stacks);
  void ClearTexture();
  void SetColor(GLColor color);
  void SetCheckerboard();
  void SetTexture(GLuint texId);
  void DrawCanonicalObject();

 protected:
  double        base_radius_;
  double        top_radius_;
  double        height_;
  double        slices_;
  double        stacks_;
  bool          draw_caps_;
  bool          owns_texture_;
  GLuint        texture_ID_;
  GLColor color_;
  static const int TEX_W = 64;
  static const int TEX_H = 64;
};

inline GLCylinder::GLCylinder(GLuint texId) 
{
  owns_texture_ = false;
  texture_ID_ = texId;
  draw_caps_ = true;
  base_radius_ = 1;
  top_radius_ = 1;
  height_ = 2;
  slices_ = 32;
  stacks_ = 32;

  SetCheckerboard();
}

inline GLCylinder::~GLCylinder() {
}

inline void GLCylinder::Init(
    double dBaseRadius,
    double dTopRadius,
    double dHeight_,
    int    nSlices_,
    int    nStacks_) {
  base_radius_ = dBaseRadius;
  top_radius_  = dTopRadius;
  height_     = dHeight_;
  slices_     = nSlices_;
  stacks_     = nStacks_;
}

inline void GLCylinder::ClearTexture() {
  if (texture_ID_ > 0) {
    if (owns_texture_) {
      glDeleteTextures(1, &texture_ID_);
    }
    texture_ID_ = 0;
  }
}

inline void GLCylinder::SetColor(GLColor color) {
  color_ = color;
}

inline void GLCylinder::SetCheckerboard() {
  ClearTexture();

  // Texture Map Init
  // after glTexImage2D(), array is no longer needed
  GLubyte img[TEX_W][TEX_H][3];
  for (int x = 0; x < TEX_W; x++) {
    for (int y = 0; y < TEX_H; y++) {
      GLubyte c = ((x & 16) ^ (y & 16)) ? 255 : 0;  // checkerboard
      img[x][y][0] = c;
      img[x][y][1] = c;
      img[x][y][2] = c;
    }
  }
  // Generate and bind the texture
  owns_texture_ = true;
  glGenTextures(1, &texture_ID_);
  glBindTexture(GL_TEXTURE_2D, texture_ID_);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEX_W, TEX_H, 0,
               GL_RGB, GL_UNSIGNED_BYTE, &img[0][0][0]);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

inline void GLCylinder::SetTexture(GLuint texId) {
  ClearTexture();
  owns_texture_ = false;
  texture_ID_ = texId;
}

// adapted from http://math.hws.edu/graphicsnotes/source/glutil/UVCylinder.java
inline void GLCylinder::DrawCanonicalObject() {
  color_.Apply();
  for (int j = 0; j < stacks_; j++) {
    double z1 = (height_/stacks_) * j;
    double z2 = (height_/stacks_) * (j + 1);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices_; i++) {
      double longitude = (2 * 3.14 / slices_) * i;
      double sinLong = sin(longitude);
      double cosLong = cos(longitude);
      double x = cosLong;
      double y = sinLong;
      glNormal3d(x, y, 0);
      if (owns_texture_) {
        glTexCoord2d(1.0 / slices_ * i, 1.0 / stacks_ * (j+1));
      }
      glVertex3d(base_radius_ * x, base_radius_ * y, z2);
      if (owns_texture_) {
        glTexCoord2d(1.0 / slices_ * i, 1.0 / stacks_ * j);
      }
      glVertex3d(top_radius_ * x, top_radius_ * y, z1);
    }
    glEnd();
  }
  if (draw_caps_) {
    // top
    double rings = 5;
    glNormal3d(0, 0, 1);
    for (int j = 0; j < rings; j++) {
      double d1 = (1.0 / rings) * j;
      double d2 = (1.0 / rings) * (j + 1);
      glBegin(GL_QUAD_STRIP);
      for (int i = 0; i <= slices_; i++) {
        double angle = (2 * 3.14 / slices_) * i;
        double dsin = sin(angle);
        double dcos = cos(angle);
        if (owns_texture_)
          glTexCoord2d(0.5 * (1 + dcos * d1), 0.5 * (1 + dsin * d1));
        glVertex3d(top_radius_ * dcos * d1, top_radius_ * dsin * d1, height_);
        if (owns_texture_)
          glTexCoord2d(0.5 * (1 + dcos * d2), 0.5 * (1 + dsin * d2));
        glVertex3d(top_radius_ * dcos * d2, top_radius_ * dsin * d2, height_);
      }
      glEnd();
    }
    // bottom
    glNormal3d(0, 0, -1);
    for (int j = 0; j < rings; j++) {
      double d1 = (1.0 / rings) * j;
      double d2 = (1.0 / rings) * (j + 1);
      glBegin(GL_QUAD_STRIP);
      for (int i = 0; i <= slices_; i++) {
        double angle = (2 * 3.14 / slices_) * i;
        double dsin = sin(angle);
        double dcos = cos(angle);
        if (owns_texture_) {
          glTexCoord2d(0.5 * (1 + dcos * d2), 0.5 * (1 + dsin * d2));
        }
        glVertex3d(base_radius_ * dcos * d2, base_radius_ * dsin * d2, 0);
        if (owns_texture_) {
          glTexCoord2d(0.5 * (1 + dcos * d1), 0.5 * (1 + dsin * d1));
        }
        glVertex3d(base_radius_ * dcos * d1, base_radius_ * dsin * d1, 0);
      }
      glEnd();
    }
  }
}

}  // namespace SceneGraph

#endif  // SCENEGRAPH_GLCYLINDER_H_
