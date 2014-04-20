#ifndef _GL_GRID_H_
#define _GL_GRID_H_

#include <SceneGraph/GLObject.h>
#include <SceneGraph/GLColor.h>
#include <pangolin/gldraw.h>

namespace SceneGraph {

class GLGrid : public GLObject {
 public:
  GLGrid(int numLines = 50, float lineSpacing = 2.0, bool perceptable = false);

  // from mvl dispview
  static void DrawGridZ0(bool filled, int numLines, float lineSpacing,
                         GLColor colorPlane, GLColor colorLines,
                         int num_minor_lines, GLColor minor_color);
  void DrawCanonicalObject();

  void SetNumLines(int nNumLines) {
    num_lines_ = nNumLines;
    ComputeBounds();
  }

  void SetLineSpacing(float fLineSpacing) {
    line_spacing_ = fLineSpacing;
    ComputeBounds();
  }

  void SetColors(const GLColor& planeColor, const GLColor& lineColor) {
    color_plane_ = planeColor;
    major_color_lines_ = lineColor;
  }

  void SetPlaneColor(const GLColor& color) {
    color_plane_ = color;
  }

  void SetLineColor(const GLColor& color) {
    major_color_lines_ = color;
  }

  void set_has_minor_lines(bool minor) {
    has_minor_lines_ = minor;
  }

  bool has_minor_lines() {
    return has_minor_lines_;
  }

  void set_num_minor_lines(int num) {
    num_minor_ = num;
  }

  int num_minor_lines() {
    return num_minor_;
  }

  void SetPlane(const Eigen::Vector3d& nd_o);

 protected:
  void ComputeBounds();

 private:
  int num_lines_;
  float line_spacing_;

  GLColor color_plane_;
  GLColor major_color_lines_;
  GLColor minor_color_lines_;

  // Plane to object transform (represents canonical basis for plane)
  Eigen::Matrix4d t_op_;

  bool has_minor_lines_;

  // Number of minor grid lines between major lines
  int num_minor_;
};
} // namespace SceneGraph

#endif
