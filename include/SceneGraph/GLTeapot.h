#pragma once

#ifndef ANDROID
#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif  // __APPLE_CC__
#endif  // ANDROID

#include <math.h>
#include <SceneGraph/GLObject.h>

namespace SceneGraph {

/** Draws a glutSolidTeapot with scale == scale[0] of GLObject */
class GLTeapot : public GLObject {
 public:
  GLTeapot() {}

  void DrawCanonicalObject() {
#ifndef ANDROID
    glutSolidTeapot(1);
#endif  // ANDROID
  }
};
}  // namespace SceneGraph
