#ifndef _GLCUBE_H_
#define _GLCUBE_H_

#include <SceneGraph/GLObject.h>
#include <math.h>

namespace SceneGraph {
class GLCube : public GLObject {
 public:
  GLCube();
  GLCube(GLuint texId);
  void ClearTexture();
  void SetCheckerboard();
  void SetTexture(GLuint texId);
  void DrawCanonicalObject();

 protected:
  const static int TEX_W = 64;
  const static int TEX_H = 64;

  bool   m_bOwnsTexture;
  GLuint m_nTexID;
};
}  // namespace SceneGraph
#endif //_GLCube_H_
