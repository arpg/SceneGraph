#pragma once

#include <glut.h>
#include <math.h>
#include <SceneGraph/GLObject.h>

namespace SceneGraph {

/** Draws a glutSolidTeapot with scale == scale[0] of GLObject */
class GLTeapot : public GLObject {
 public:
  GLTeapot() {}

  void DrawCanonicalObject() {
    glutSolidTeapot(m_dScale[0]);
  }
};
}  // namespace SceneGraph
