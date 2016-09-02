#ifndef _GL_GRID_H_
#define _GL_GRID_H_

#include <SceneGraph/GLObject.h>
#include <SceneGraph/GLColor.h>
#include <pangolin/gl/gldraw.h>

namespace SceneGraph {

class GLGrid : public GLObject {
 public:
  GLGrid(int num_lines = 50,
         float line_spacing = 2.0,
         bool perceptable = false);

  /** Draw a grid centered at (0, 0, 0) with a specified number of
   * lines in each direction.
   *
   * @param filled Should the grid squares be filled in
   * @param num_lines_neg_x # of lines to draw in the -x direction
   * @param num_lines_x # of lines to draw in the +x direction
   * @param num_lines_neg_y # of lines to draw in the -y direction
   * @param num_lines_y # of lines to draw in the +y direction
   * @param line_spacing The space between major grid lines
   * @param color_plane Color of filled grid squares. Ignored if filled == false
   * @param major_color Color of major grid lines
   * @param num_minor_lines Number of minor grid lines between each major line
   * @param minor_color Color for minor grid lines
   */
  static void DrawGridZ0(
      bool filled,
      int num_lines_neg_x, int num_lines_x,
      int num_lines_neg_y, int num_lines_y,
      float line_spacing, GLColor color_plane, GLColor major_color,
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

  /** Set the number of minor lines between major lines */
  void set_num_minor_lines(int num) {
    num_minor_ = num;
  }

  /** Number of minor lines between major lines */
  int num_minor_lines() {
    return num_minor_;
  }

  /** nd_o = (n_o / d_o)
   *
   * n_o : Normal of plane with respect to object frame of ref
   * d_o : Distance of closest approach with origin of object frame
   */
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
