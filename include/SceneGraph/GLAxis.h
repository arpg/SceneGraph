// Copyright 2014 ARPG

#ifndef SCENEGRAPH_GLAXIS_H_
#define SCENEGRAPH_GLAXIS_H_

#include <SceneGraph/GLObject.h>

namespace SceneGraph {

class SCENEGRAPH_EXPORT GLAxis : public GLObject {
 public:
  GLAxis(const float axisSize = 1.0f, const bool bPretty = false);
  virtual ~GLAxis();
  static void DrawAxis(float fScale = 1.0f);
  void DrawCanonicalObject();
  void SetAxisSize(float fScale) { m_fAxisScale = fScale; }
  float GetAxisSize() { return m_fAxisScale; }
  bool IsSelectable();
 protected:
  float     m_fAxisScale;
  bool      m_bPretty;
  int       m_iAxisID;
};

}  // namespace SceneGraph

#endif  // SCENEGRAPH_GLAXIS_H_
