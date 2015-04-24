#pragma once

#include <SceneGraph/GLColor.h>
#include <SceneGraph/GLObject.h>

namespace SceneGraph {
class GLDynamicGrid : public GLObject {
 public:
  GLDynamicGrid();
  virtual ~GLDynamicGrid() {}

  virtual void DrawCanonicalObject();

  /** The spacing between major lines */
  void set_line_spacing(float spacing);
  float line_spacing();

  /** Number of minor lines between major lines */
  void set_num_minor_lines(int num);
  int num_minor_lines();

  void set_major_color(GLColor color);
  GLColor major_color();

  void set_minor_color(GLColor color);
  GLColor minor_color();

  /** nd_o = (n_o / d_o)
   *
   * n_o : Normal of plane with respect to object frame of ref
   * d_o : Distance of closest approach with origin of object frame
   */
  void set_normal(const Eigen::Vector3d& nd_o);

  void set_bounds(const AxisAlignedBoundingBox& bbox) {
    m_aabb = bbox;
    ComputeGridProperties();
  }

 protected:
  void ComputeGridProperties();

 private:
  float spacing_;
  int num_minor_lines_;
  GLColor major_color_, minor_color_;

  // Plane to object transform (represents canonical basis for plane)
  Eigen::Matrix4d t_op_;

  // Grid properties
  int num_neg_x_, num_pos_x_, num_neg_y_, num_pos_y_;
};
}  // namespace SceneGraph
