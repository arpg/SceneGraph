#ifndef GLAXIS_H
#define GLAXIS_H

#include <SceneGraph/GLObject.h>

namespace SceneGraph {

class SCENEGRAPH_EXPORT GLAxis : public GLObject {
 public:
  GLAxis(const float axisSize = 1.0f, const bool bPretty = false);
  virtual ~GLAxis();

  static void DrawAxis(float fScale = 1.0f);
  void DrawSolidAxis(float fScale = 1.0f);
  void DrawCanonicalObject();

  void SetAxisSize(float fScale) {
    m_fAxisScale =  fScale ;
  }

  float GetAxisSize() { return m_fAxisScale; }

 protected:
  float m_fAxisScale;
  bool m_bPretty;
  GLUquadric*   m_pQuadric;
};

} // SceneGraph

#endif // GLAXIS_H
