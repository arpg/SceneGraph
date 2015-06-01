// Copyright 2014 ARPG

#include <SceneGraph/GLAxis.h>

namespace SceneGraph {

GLAxis::GLAxis(const float axisSize, const bool bPretty)
    : GLObject("Axis"), m_fAxisScale(axisSize), m_bPretty(bPretty) {
  m_iAxisID = AllocSelectionId();
  m_bPerceptable = false;
  m_aabb.SetZero();
}

GLAxis::~GLAxis() {
}

bool GLAxis::IsSelectable() {
  return m_bIsSelectable;
}

void GLAxis::DrawAxis(float fScale) {
  // Draw axis
  glEnableClientState(GL_VERTEX_ARRAY);

  GLfloat axis[] = {
    0.0, 0.0, 0.0,
    fScale , 0, 0 };
  glColor4f(1, 0, 0, 1);
  glVertexPointer(3, GL_FLOAT, 0, axis);
  glDrawArrays(GL_LINE_STRIP, 0, 2);

  axis[3] = 0.0;
  axis[4] = fScale;
  glColor4f(0, 1, 0, 1);
  glVertexPointer(3, GL_FLOAT, 0, axis);
  glDrawArrays(GL_LINE_STRIP, 0, 2);

  axis[4] = 0.0;
  axis[5] = fScale;
  glColor4f(0, 0, 1, 1);
  glVertexPointer(3, GL_FLOAT, 0, axis);
  glDrawArrays(GL_LINE_STRIP, 0, 2);

  glDisableClientState(GL_VERTEX_ARRAY);
}

void GLAxis::DrawCanonicalObject() {
  glPushName( m_iAxisID );
  DrawAxis(m_fAxisScale);
  glPopName();
}

}  // namespace SceneGraph
