#ifndef SCENEGRAPH_GLHEIGHTMAP_H_
#define SCENEGRAPH_GLHEIGHTMAP_H_

#include "SceneGraph/GLObject.h"

namespace SceneGraph {
/// The SceneGraph heightmap
class GLHeightmap : public GLObject {
public:

  GLHeightmap(std::vector<double> x_array, std::vector<double> y_array,
              std::vector<double> z_array,
              double row_count, double col_count) {
    X_ = x_array;
    Y_ = y_array;
    Z_ = z_array;
    row_count_ = row_count;
    col_count_ = col_count;
    render_ = false;
  }

  /////////////////////////////////////////////////////////////////////////////
  void DrawCanonicalObject() {
    if (render_) {
      glBegin(GL_TRIANGLES); //  Render Polygons
    } else {
      glBegin(GL_LINES); //  Render Lines Instead
    }
    int index = 0;
    for (int i = 0; i<row_count_-1; i++) {
      //  Right now, I can't get the max height, so I color
      //  the mesh lighter as we go down the rows.
      float color_change = i/row_count_;
      glColor3f(1, color_change, 1);
      for (int j = 0; j<col_count_-1; j++) {
        //  Triangle 1
        index = j*row_count_+i;
        glVertex3f(X_[index], Y_[index], Z_[index]);
        index = j*row_count_+i+1;
        glVertex3f(X_[index], Y_[index], Z_[index]);
        index = (j+1)*row_count_+i+1;
        glVertex3f(X_[index], Y_[index], Z_[index]);

        //  Triangle 2
        index = j*row_count_+i;
        glVertex3f(X_[index], Y_[index], Z_[index]);
        index = (j+1)*row_count_+i+1;
        glVertex3f(X_[index], Y_[index], Z_[index]);
        index = (j+1)*row_count_+i;
        glVertex3f(X_[index], Y_[index], Z_[index]);
      }
    }
    glEnd();
    //  Reset color
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  }

private:
  std::vector<double> X_;
  std::vector<double> Y_;
  std::vector<double> Z_;
  double row_count_;
  double col_count_;
  bool render_;
};

}

#endif  // SCENEGRAPH_GLHEIGHTMAP_H_
