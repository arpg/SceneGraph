#include <SceneGraph/GLGrid.h>

namespace SceneGraph {
GLGrid::GLGrid(int numLines, float lineSpacing, bool perceptable)
    : GLObject("Grid"),
      num_lines_(numLines),
      line_spacing_(lineSpacing),
      color_plane_(0.4f, 0.4f, 0.4f, 1.0f),
      major_color_lines_(0.5f, 0.5f, 0.5f, 1.0f),
      minor_color_lines_(0.5f, 0.5f, 0.5f, 0.5f),
      has_minor_lines_(false),
      num_minor_(5) {
  m_bPerceptable = perceptable;
  t_op_ = Eigen::Matrix4d::Identity();
  ComputeBounds();
}

// from mvl dispview
void GLGrid::DrawGridZ0(bool filled, int numLines, float lineSpacing,
                        GLColor colorPlane, GLColor colorLines,
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

  const float halfsize = lineSpacing * numLines;

  if (filled) {
    colorPlane.Apply();
    glRectf(-halfsize, -halfsize, halfsize, halfsize);

    // Don't overwrite this depth when drawing lines:
    gl.glDepthMask(GL_FALSE);
  }

  float minor_spacing = lineSpacing / num_minor_lines;
  for (int i = -numLines; i <= numLines; i++) {
    float major_pos = i * lineSpacing;
    if (i < numLines) {
      float minor_pos = major_pos;
      minor_color.Apply();
      for (int j = 0; j < num_minor_lines; ++j) {
        minor_pos += minor_spacing;
        pangolin::glDrawLine(halfsize, minor_pos, 0.0,
                             -halfsize, minor_pos, 0.0);
        pangolin::glDrawLine(minor_pos,  halfsize, 0.0,
                             minor_pos, -halfsize, 0.0);
      }
    }

    if (!i) continue;
    colorLines.Apply();
    pangolin::glDrawLine(halfsize, major_pos, 0.0,
                         -halfsize, major_pos, 0.0);
    pangolin::glDrawLine(major_pos,  halfsize, 0.0,
                         major_pos, -halfsize, 0.0);
  }

  glColor4ub(255, 0, 0, 128);
  pangolin::glDrawLine(halfsize , 0.0, 0.0, -halfsize , 0.0, 0.0);

  glColor4ub(0, 255, 0, 128);
  pangolin::glDrawLine(0.0,  halfsize, 0.0,  0.0, -halfsize, 0.0);
}

void GLGrid::DrawCanonicalObject(void) {
  glPushMatrix();
  glMultMatrixd(t_op_.data());
  DrawGridZ0(m_bPerceptable, num_lines_, line_spacing_,
             color_plane_, major_color_lines_,
             has_minor_lines_ ? num_minor_ : 0,
             minor_color_lines_);
  glPopMatrix();
}

// nd_o = (n_o / d_o)
// n_o : Normal of plane with respect to object frame of ref
// d_o : Distance of closest approach with origin of object frame
void GLGrid::SetPlane(const Eigen::Vector3d& nd_o) {
  const double d = 1.0 / nd_o.norm();
  const Eigen::Vector3d n = d * nd_o;
  t_op_.block<3,3>(0,0) = Rotation_a2b(Eigen::Vector3d(0,0,-1),n);
  t_op_.block<3,1>(0,3) = -d*n;
  ComputeBounds();
}

void GLGrid::ComputeBounds() {
  const float halfsize = line_spacing_*num_lines_;
  m_aabb.Clear();
  m_aabb.Insert(t_op_,Eigen::Vector3d(-halfsize,-halfsize,0));
  m_aabb.Insert(t_op_,Eigen::Vector3d(-halfsize,halfsize,0));
  m_aabb.Insert(t_op_,Eigen::Vector3d(halfsize,-halfsize,0));
  m_aabb.Insert(t_op_,Eigen::Vector3d(halfsize,halfsize,0));
}
} // namespace SceneGraph
