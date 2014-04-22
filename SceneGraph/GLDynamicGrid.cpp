#include <SceneGraph/GLDynamicGrid.h>
#include <SceneGraph/GLGrid.h>

namespace SceneGraph {

GLDynamicGrid::GLDynamicGrid() : spacing_(10.0),
                                 num_minor_lines_(5),
                                 major_color_(0.5f, 0.5f, 0.5f, 1.0f),
                                 minor_color_(0.5f, 0.5f, 0.5f, 0.5f) {
  t_op_.setIdentity();
  set_bounds({{-20., -20., 0.}, {20., 20., 0.}});
}

void GLDynamicGrid::ComputeGridProperties() {
  Eigen::Vector3d bmax = m_aabb.Max();
  Eigen::Vector3d bmin = m_aabb.Min();

  double minx = std::min(bmin[0], bmax[0]);
  double miny = std::min(bmin[1], bmax[1]);

  double maxx = std::max(bmin[0], bmax[0]);
  double maxy = std::max(bmin[1], bmax[1]);

  num_neg_x_ = minx < 0 ? std::ceil(std::abs(minx / spacing_)) : 1;
  num_pos_x_ = maxx > 0 ? std::ceil(std::abs(maxx / spacing_)) : 1;

  num_neg_y_ = miny < 0 ? std::ceil(std::abs(miny / spacing_)) : 1;
  num_pos_y_ = maxy > 0 ? std::ceil(std::abs(maxy / spacing_)) : 1;
}

void GLDynamicGrid::DrawCanonicalObject() {
  glPushMatrix();
  glMultMatrixd(t_op_.data());
  GLGrid::DrawGridZ0(false,
                     num_neg_x_, num_pos_x_,
                     num_neg_y_, num_pos_y_,
                     spacing_, GLColor(), major_color_,
                     num_minor_lines_, minor_color_);
  glPopMatrix();
}

void GLDynamicGrid::set_normal(const Eigen::Vector3d& nd_o) {
  const double d = 1.0 / nd_o.norm();
  const Eigen::Vector3d n = d * nd_o;
  t_op_.block<3,3>(0,0) = Rotation_a2b(Eigen::Vector3d(0,0,-1),n);
  t_op_.block<3,1>(0,3) = -d*n;
}

/** The spacing between major lines */
void GLDynamicGrid::set_line_spacing(float spacing) {
  spacing_ = spacing;
  ComputeGridProperties();
}

float GLDynamicGrid::line_spacing() {
  return spacing_;
}

/** Number of minor lines between major lines */
void GLDynamicGrid::set_num_minor_lines(int num) {
  num_minor_lines_ = num;
}

int GLDynamicGrid::num_minor_lines() {
  return num_minor_lines_;
}


void GLDynamicGrid::set_major_color(GLColor color) {
  major_color_ = color;
}

GLColor GLDynamicGrid::major_color() {
  return major_color_;
}


void GLDynamicGrid::set_minor_color(GLColor color) {
  minor_color_ = color;
}

GLColor GLDynamicGrid::minor_color() {
  return minor_color_;
}
}  // namespace SceneGraph
