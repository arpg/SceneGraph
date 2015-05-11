#ifndef _SCENEGRAPH_GLCOLOR_H__
#define _SCENEGRAPH_GLCOLOR_H__

#include <sstream>
#include <cstdio>
#include <cmath>
#include <Eigen/Eigen>
#include <SceneGraph/GLHelpers.h>

namespace SceneGraph {

struct GLColor {
  static GLColor HsvColor(double hue, double s = 1.0, double v = 1.0) {
    hue *= 360.0;
    const double h = hue / 60.0;
    const int i = floor(h);
    const double f = (i % 2 == 0) ? 1 - (h - i) : h - i;
    const double m = v * (1-s);
    const double n = v * (1-s*f);
    GLColor color;
    switch(i) {
      case 0: color = GLColor(v, n, m, 1.0f); break;
      case 1: color = GLColor(n, v, m, 1.0f); break;
      case 2: color = GLColor(m, v, n, 1.0f); break;
      case 3: color = GLColor(m, n, v, 1.0f); break;
      case 4: color = GLColor(n, m, v, 1.0f); break;
      case 5: color = GLColor(v, m, n, 1.0f); break;
      default:
        break;
    }
    return color;
  }

  GLColor(const Eigen::Vector4d& rgba) {
    r = rgba[0];
    g = rgba[1];
    b = rgba[2];
    a = rgba[3];
  }

  GLColor(float tr = 1.0f, float tg = 1.0f, float tb = 1.0f, float ta = 1.0f) {
    r = tr;
    g = tg;
    b = tb;
    a = ta;
  }

  GLColor(int tr, int tg, int tb, int ta = 255) {
    r = tr/255.0f;
    g = tg/255.0f;
    b = tb/255.0f;
    a = ta/255.0f;
  }

  void Apply() const {
    glColor4f(r,g,b,a);
  }

  struct {
    float r;
    float g;
    float b;
    float a;
  };
};

#ifndef NAME_MAX
#   define NAME_MAX 1024
#endif

/// All types you wish to use with CVars must overload << and >>.
inline std::ostream &operator<<(std::ostream &stream, GLColor &color) {
  int r = int(255*color.r);
  int g = int(255*color.g);
  int b = int(255*color.b);
  int a = int(255*color.a);

  stream << "[ " << r << ",  " << g << ",  " << b << ",  " << a << " ]";
  return stream;
}

/// All types you wish to use with CVars must overload << and >>.
inline std::istream &operator>>(std::istream &stream, GLColor &color) {
  int r = 0, g = 0, b = 0, a = 0;

  char str[NAME_MAX] = {0};
  stream.readsome(str, NAME_MAX);
  sscanf(str, "[ %d, %d, %d, %d ]", &r, &g, &b, &a);

  color.r = (float)r/255.0f;
  color.g = (float)g/255.0f;
  color.b = (float)b/255.0f;
  color.a = (float)a/255.0f;

  return stream;
}
} // namespace SceneGraph
#endif
