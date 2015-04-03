#include <SceneGraph/GLGrid.h>

namespace SceneGraph {
GLGrid::GLGrid(int num_lines, float line_spacing, bool perceptable)
    : GLObject("Grid"),
      num_lines_(num_lines),
      line_spacing_(line_spacing),
      color_plane_(0.4f, 0.4f, 0.4f, 1.0f),
      major_color_lines_(0.5f, 0.5f, 0.5f, 1.0f),
      minor_color_lines_(0.5f, 0.5f, 0.5f, 0.5f),
      has_minor_lines_(false),
      num_minor_(5) {
  m_bPerceptable = perceptable;
  t_op_ = Eigen::Matrix4d::Identity();
  ComputeBounds();
}


// along_x or along Y axis
inline void DrawMajorGridLines(
    int num_neg, int num_pos, float spacing,
    float min, float max, bool along_x) {
  for (int i = -num_neg; i <= num_pos; i++) {
    if (!i) continue;
    float major_pos = i * spacing;
    if (along_x) {
      pangolin::glDrawLine(max, major_pos, 0.0,
                           min, major_pos, 0.0);
    } else {
      pangolin::glDrawLine(major_pos, max, 0.0,
                           major_pos, min, 0.0);
    }
  }
}

inline void DrawMinorGridLines(int num_major_neg,
                               int num_major_pos,
                               int num_minor_lines,
                               float major_spacing,
                               float min, float max,
                               bool along_x) {
  float minor_spacing = major_spacing / (num_minor_lines + 1);

  for (int i = -num_major_neg; i < num_major_pos; i++) {
    float minor_pos = i * major_spacing;
    for (int j = 0; j < num_minor_lines; ++j) {
      minor_pos += minor_spacing;
      if (along_x) {
        pangolin::glDrawLine(max, minor_pos, 0.0,
                             min, minor_pos, 0.0);
      } else {
        pangolin::glDrawLine(minor_pos, max, 0.0,
                             minor_pos, min, 0.0);
      }
    }
  }
}

// from mvl dispview
void GLGrid::DrawGridZ0(
    bool filled,
    int num_lines_neg_x, int num_lines_x,
    int num_lines_neg_y, int num_lines_y,
    float line_spacing,
    GLColor color_plane, GLColor major_color,
    int num_minor_lines, GLColor minor_color) {
  pangolin::GlState gl;
#ifdef ANDROID
  gl.glDisable(GL_LINE_SMOOTH);
#endif

  // Prevent Z-Fighting between plane and lines
  glPolygonOffset(1.0, 1.0);
  gl.glEnable(GL_POLYGON_OFFSET_FILL);
  gl.glDisable(GL_CULL_FACE);

  //            glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
  GLfloat ambient[4] = {1,1,1,1};
  GLfloat diffuse[4] = {0,0,0,1};
  GLfloat specular[4] = {0,0,0,1};
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);

  glNormal3f(0,0,-1);

  float minx = -(num_lines_neg_x * line_spacing);
  float maxx = num_lines_x * line_spacing;
  float miny = -(num_lines_neg_y * line_spacing);
  float maxy = num_lines_y * line_spacing;

  if (filled) {
    color_plane.Apply();
    glRectf(minx, miny, maxx, maxy);

    // Don't overwrite this depth when drawing lines:
    gl.glDepthMask(GL_FALSE);
  }

  major_color.Apply();
  DrawMajorGridLines(num_lines_neg_x, num_lines_x,
                     line_spacing, maxy, miny, false);
  DrawMajorGridLines(num_lines_neg_y, num_lines_y,
                     line_spacing, maxx, minx, true);

  minor_color.Apply();
  DrawMinorGridLines(num_lines_neg_x, num_lines_x, num_minor_lines,
                     line_spacing, maxy, miny, false);
  DrawMinorGridLines(num_lines_neg_y, num_lines_y, num_minor_lines,
                     line_spacing, maxx, minx, true);

  glColor4ub(255, 0, 0, 128);
  pangolin::glDrawLine(minx, 0.0, 0.0, maxx , 0.0, 0.0);

  glColor4ub(0, 255, 0, 128);
  pangolin::glDrawLine(0.0, miny, 0.0, 0.0, maxy, 0.0);
}

void GLGrid::DrawCanonicalObject(void) {
  glPushMatrix();
  glMultMatrixd(t_op_.data());
  DrawGridZ0(m_bPerceptable,
             num_lines_ / 2, num_lines_ / 2, num_lines_ / 2, num_lines_ / 2,
             line_spacing_,
             color_plane_, major_color_lines_,
             has_minor_lines_ ? num_minor_ : 0,
             minor_color_lines_);
  glPopMatrix();
}

void GLGrid::SetPlane(const Eigen::Vector3d& nd_o) {
  const double d = 1.0 / nd_o.norm();
  const Eigen::Vector3d n = d * nd_o;
  t_op_.block<3,3>(0,0) = Rotation_a2b(Eigen::Vector3d(0,0,-1),n);
  t_op_.block<3,1>(0,3) = -d*n;
  ComputeBounds();
}

void GLGrid::ComputeBounds() {
  const float halfsize_x = line_spacing_*num_lines_;
  m_aabb.Clear();
  m_aabb.Insert(t_op_, Eigen::Vector3d(-halfsize_x, -halfsize_x, 0));
  m_aabb.Insert(t_op_, Eigen::Vector3d(-halfsize_x, halfsize_x, 0));
  m_aabb.Insert(t_op_, Eigen::Vector3d(halfsize_x, -halfsize_x, 0));
  m_aabb.Insert(t_op_, Eigen::Vector3d(halfsize_x, halfsize_x, 0));
}
} // namespace SceneGraph
