#ifndef _GL_LINESTRIP_H_
#define _GL_LINESTRIP_H_

#include <SceneGraph/GLObject.h>
#include <SceneGraph/GLColor.h>
#include <SceneGraph/PangolinGlCachedSizeableBuffer.h>

namespace SceneGraph {

class GLLineStrip : public GLObject {
 public:
  GLLineStrip()
      : GLObject("LineStrip"),
        m_buffer(pangolin::GlArrayBuffer, 100, GL_FLOAT, 3, GL_DYNAMIC_DRAW) {
    m_bPerceptable = false;
    m_line_width = 1;
  }

  void InitReset() { m_Color = GLColor(); }

  void DrawCanonicalObject() {
    m_Color.Apply();
    m_buffer.Bind();
    glVertexPointer(m_buffer.count_per_element, m_buffer.datatype, 0, 0);
    glLineWidth(m_line_width);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_LINE_STRIP, m_buffer.start(), m_buffer.size());
    glDisableClientState(GL_VERTEX_ARRAY);
    m_buffer.Unbind();
  }

  void SetPoint(Eigen::Vector3d Point) { m_buffer.Add(Point.cast<float>()); }

  void SetPoint(double P[3]) {
    m_buffer.Add(Eigen::Vector3f(P[0], P[1], P[2]));
  }

  void SetPoint(double X = 0, double Y = 0, double Z = 0) {
    m_buffer.Add(Eigen::Vector3f(X, Y, Z));
  }

  void SetPoints(const std::vector<double>& vPts) {
    m_buffer.Clear();
    for (size_t i = 0; i < vPts.size(); i += 3) {
      SetPoint(vPts[i], vPts[i + 1], vPts[i + 2]);
    }
  }

  void SetPointsFromTrajectory(const Eigen::Vector6dAlignedVec& vPts) {
    m_buffer.Clear();
    for (size_t i = 0; i < vPts.size(); ++i) {
      SetPoint(vPts[i][0], vPts[i][1], vPts[i][2]);
    }
  }

  void SetPointsFromTrajectory(const Eigen::Vector3dAlignedVec& vPts) {
    m_buffer.Clear();
    for (size_t i = 0; i < vPts.size(); ++i) {
      SetPoint(vPts[i][0], vPts[i][1], vPts[i][2]);
    }
  }

  // deprecated: set GLLineStrip pose instead.
  void SetReference(unsigned int Xref, unsigned int Yref, unsigned int Zref) {
    m_T_po(0, 3) = Xref;
    m_T_po(1, 3) = Yref;
    m_T_po(2, 3) = Zref;
  }

  void ClearLines() { m_buffer.Clear(); }

  void SetColor(const GLColor& c) { m_Color = c; }

  void SetLineWidth(double width) { m_line_width = width; }

  void SetColor(double r, double g, double b, double a) {
    m_Color.r = r;
    m_Color.g = g;
    m_Color.b = b;
    m_Color.a = a;
  }

 private:
  GlCachedSizeableBuffer m_buffer;
  GLColor m_Color;
  double m_line_width;
};
}

#endif
