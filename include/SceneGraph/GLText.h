#ifndef _GL_TEXT_H_
#define _GL_TEXT_H_

#include <pangolin/gl/glinclude.h>
#include <pangolin/gl/glfont.h>

#include <SceneGraph/GLObject.h>
#include <SceneGraph/GLColor.h>

#include <string>

namespace SceneGraph {

class GLText : public GLObject {
 public:
  static pangolin::GlFont& GetFont() {
    return pangolin::GlFont::I();
  }

  /////////////////////////////////
  // Static Utilities for drawing text
  /////////////////////////////////

  /// Render text at current glRasterPos
  static void Draw(const std::string& text) {
    pangolin::GlText txt = GetFont().Text(text.c_str());
    txt.Draw();
  }

  /// Render text at (x,y,z)
  static void Draw(const std::string& text, float x, float y, float z = 0) {
    pangolin::GlText txt = GetFont().Text(text.c_str());
    txt.Draw(x,y,z);
  }

  /// Return width (in pixels) of text
  static int Width(const std::string& text) {
    pangolin::GlText txt = GetFont().Text(text.c_str());
    return txt.Width();
  }

  /////////////////////////////////
  // GLText constructors
  /////////////////////////////////

  GLText(std::string text = "", double x = 0, double y = 0, double z = 0)
      : GLObject("Text: " + text), m_sText(text), viewpoint_invariant_(true) {
    m_bPerceptable = false;
    SetPosition(x,y,z);
  }

  /////////////////////////////////
  // GLText member functions
  /////////////////////////////////

  void DrawCanonicalObject() {
    if(m_gltext.Text() != m_sText) {
      // Reinitialise text
      m_gltext = GetFont().Text(m_sText.c_str());
    }

    m_Color.Apply();
    pangolin::GlState gl;
    gl.glDisable(GL_DEPTH_TEST);
    gl.glDisable(GL_LIGHTING);
    if (viewpoint_invariant_) {
      Draw(m_sText, 0, 0, 0);
    } else {
      Draw(m_sText);
    }
  }

  void SetText(const std::string& sText) {
    m_sText = sText;
  }

  void ClearText() {
    m_sText.clear();
  }

  void set_color(const GLColor& color) {
    m_Color = color;
  }

  /** Configure if the GLText is 3D or attach to the window as a 2D object */
  void set_viewpoint_invariant(bool is) {
    viewpoint_invariant_ = is;
  }

 private:
  std::string m_sText;
  pangolin::GlText m_gltext;
  GLColor m_Color;
  bool viewpoint_invariant_;
};
} // SceneGraph

#endif
