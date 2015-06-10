// Copyright 2014 ARPG

#ifndef SCENEGRAPH_PANGOLINSCENEGRAPHHANDLER_H_
#define SCENEGRAPH_PANGOLINSCENEGRAPHHANDLER_H_

#pragma once

#include <pangolin/simple_math.h>
#include <pangolin/pangolin.h>
#include <SceneGraph/SceneGraph.h>
#include <map>
#include <limits>

namespace SceneGraph {

struct HandlerSceneGraph : pangolin::Handler3D {
  HandlerSceneGraph(SceneGraph::GLSceneGraph& graph,
                    pangolin::OpenGlRenderState& cam_state,
                    pangolin::AxisDirection enforce_up = pangolin::AxisNone,
                    float trans_scale = 0.01f)
      : pangolin::Handler3D(cam_state, enforce_up, trans_scale),
        m_scenegraph(graph), m_grab_width(15) {}

#ifdef HAVE_GLES
  // Return lambda parameter of ray intersection with obj.
  // returns numeric_limits<GLdouble>::max() when no intersection.
  GLdouble Intersect(const GLObject& obj, LineSegment<GLdouble> ray) {
    GLdouble bestl = std::numeric_limits<GLdouble>::max();

    const LineSegment<GLdouble> ray_o =
        obj.GetPose4x4_op().cast<GLdouble>() * ray;
    GLdouble l = obj.ObjectBounds().RayIntersect(ray_o);
    if (l < bestl) bestl = l;

    for (size_t c = 0; c < obj.NumChildren(); ++c) {
      l = Intersect(obj[c], ray_o);
      if (l < bestl) bestl = l;
    }
    return bestl;
  }

  // Override GetPosNormal in pangolin::Handler3D
  // since it is useless on Android.
  void GetPosNormal(pangolin::View& view, int x, int y,
                    GLdouble p[3], GLdouble Pw[3], GLdouble Pc[3],
                    GLdouble n[3], GLdouble default_z = 1.0) {
    CheckGlDieOnError();

    const GLint viewport[4] = {view.v.l, view.v.b, view.v.w, view.v.h};
    const pangolin::OpenGlMatrix proj = cam_state->GetProjectionMatrix();
    const pangolin::OpenGlMatrix mv = cam_state->GetModelViewMatrix();
    const pangolin::OpenGlMatrix mvinv = mv.Inverse();

    Eigen::Matrix<GLdouble, 3, 1> Pw1(mvinv.m[12], mvinv.m[13], mvinv.m[14]);
    Eigen::Matrix<GLdouble, 3, 1> Pw2;
    gluUnProject(x, y, 0.1f, mv.m, proj.m, viewport, &Pw2[0], &Pw2[1], &Pw2[2]);

    LineSegment<GLdouble> ray(Pw1, Pw2);
    GLdouble lambda = Intersect(m_scenegraph, ray);

    if (lambda < std::numeric_limits<GLdouble>::max()) {
      Eigen::Map<Eigen::Matrix<GLdouble, 3, 1> > ePw(Pw);
      ePw = ray(lambda);
    } else {
      gluUnProject(x, y, default_z, mv.m, proj.m,
                   viewport, &Pw[0], &Pw[1], &Pw[2]);
    }

    gluProject(Pw[0], Pw[1], Pw[2], mv.m, proj.m,
               viewport, &p[0], &p[1], &p[2]);
    pangolin::LieApplySE34x4vec3(Pc, mv.m, Pw);

    // TODO: Compute normal
    n[0] = 0.0; n[1] = 0.0; n[2] = 0.0;
  }
#endif


  void ProcessHitBuffer(GLint hits, GLuint* buf,
                        std::map<int, SceneGraph::GLObject*>& objects ) {
    GLuint* closestNames = 0;
    GLuint closestNumNames = 0;
    GLuint closestZ = std::numeric_limits<GLuint>::max();
    for (int i = 0; i < hits; i++) {
      if (buf[1] < closestZ) {
        closestNames = buf + 3;
        closestNumNames = buf[0];
        closestZ = buf[1];
      }
      buf += buf[0] + 3;
    }
    for (unsigned int i = 0; i < closestNumNames; i++) {
      const int pickId = closestNames[i];
      SceneGraph::GLObject* obj = m_scenegraph.GetObject(pickId);
      if (obj) {
        objects[pickId] = obj;
      }
    }
  }

  void gluPickMatrix(GLdouble x, GLdouble y, GLdouble width, GLdouble height,
                     GLint viewport[4]) {
    GLfloat m[16];
    GLfloat sx, sy;
    GLfloat tx, ty;
    sx = viewport[2] / width;
    sy = viewport[3] / height;
    tx = (viewport[2] + 2.0 * (viewport[0] - x)) / width;
    ty = (viewport[3] + 2.0 * (viewport[1] - y)) / height;
#define M(row, col) m[col*4+row]
    M(0, 0) = sx;
    M(0, 1) = 0.0;
    M(0, 2) = 0.0;
    M(0, 3) = tx;
    M(1, 0) = 0.0;
    M(1, 1) = sy;
    M(1, 2) = 0.0;
    M(1, 3) = ty;
    M(2, 0) = 0.0;
    M(2, 1) = 0.0;
    M(2, 2) = 1.0;
    M(2, 3) = 0.0;
    M(3, 0) = 0.0;
    M(3, 1) = 0.0;
    M(3, 2) = 0.0;
    M(3, 3) = 1.0;
#undef M
    glMultMatrixf(m);
  }

  void ComputeHits(pangolin::View& display,
                   const pangolin::OpenGlRenderState& cam_state,
                   int x, int y, int grab_width,
                   std::map<int, SceneGraph::GLObject*>& hit_objects ) {
#ifndef HAVE_GLES
    // Get views viewport / modelview /projection
    GLint viewport[4] = {display.v.l, display.v.b, display.v.w, display.v.h};
    pangolin::OpenGlMatrix mv = cam_state.GetModelViewMatrix();
    pangolin::OpenGlMatrix proj = cam_state.GetProjectionMatrix();

    // Prepare hit buffer object
    const unsigned int MAX_SEL_SIZE = 64;
    GLuint vSelectBuf[MAX_SEL_SIZE];
    glSelectBuffer(MAX_SEL_SIZE, vSelectBuf);

    // Load and adjust modelview projection matrices
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPickMatrix(x, y, grab_width, grab_width, viewport);
    proj.Multiply();
    glMatrixMode(GL_MODELVIEW);
    mv.Load();

    // Render scenegraph in 'select' mode
    glRenderMode(GL_SELECT);
    glInitNames();
    m_scenegraph.DrawObjectAndChildren(eRenderSelectable);
    glFlush();

    GLint nHits = glRenderMode(eRenderVisible);
//    std::cout << " -- Number of Hits are: " << nHits << std::endl;
//    std::cout << " -- size of hitobjects: " << hit_objects.size() << std::endl;
    if (nHits > 0) {
      ProcessHitBuffer(nHits, vSelectBuf, hit_objects);
    }
#endif  // HAVE_GLES
  }

  void Mouse(pangolin::View& view, pangolin::MouseButton button,
             int x, int y, bool pressed, int button_state) {
    GetPosNormal(view, x, y, p, Pw, Pc, n);
    bool handled = false;

    if (pressed) {
      m_selected_objects.clear();
      ComputeHits(view, *cam_state, x, y, m_grab_width, m_selected_objects);
      // find the waypoint object among selected objects and setvisible to false
      // otherwise when draging object it will keep moving toward camera
      for (std::map<int, SceneGraph::GLObject*>::iterator i =
               m_selected_objects.begin();
           i != m_selected_objects.end(); ++i ) {
          std::string objname;
          i->second->GetName(objname);
          if(objname.find("Waypoint") != std::string::npos) {
//              i->second->SetVisible(false);
            GetPosNormal(view, x, y, p, Pw, Pc, n);
          }
      }

      for (std::map<int, SceneGraph::GLObject*>::iterator i =
               m_selected_objects.begin();
           i != m_selected_objects.end(); ++i ) {
        handled |= i->second->Mouse(
            button,
            Eigen::Map<Eigen::Matrix<GLdouble, 3, 1> >(p).cast<double>(),
            Eigen::Map<Eigen::Matrix<GLdouble, 3, 1> >(Pw).cast<double>(),
            Eigen::Map<Eigen::Matrix<GLdouble, 3, 1> >(n).cast<double>(),
            pressed, button_state, i->first);
      }
    } else {
        // set waypoints to visible when click released
        for (std::map<int, SceneGraph::GLObject*>::iterator i =
                 m_selected_objects.begin();
             i != m_selected_objects.end(); ++i ) {
            std::string objname;
            i->second->GetName(objname);
            if(objname.find("Waypoint") != std::string::npos) {
//              i->second->SetVisible(true);
            }
        }
      for (std::map<int, SceneGraph::GLObject*>::iterator i =
               m_selected_objects.begin();
           i != m_selected_objects.end(); ++i ) {
        handled |= i->second->Mouse(
            button,
            Eigen::Map<Eigen::Matrix<GLdouble, 3, 1> >(p).cast<double>(),
            Eigen::Map<Eigen::Matrix<GLdouble, 3, 1> >(Pw).cast<double>(),
            Eigen::Map<Eigen::Matrix<GLdouble, 3, 1> >(n).cast<double>(),
            pressed, button_state, i->first);
      }
    }

    if (!handled) {
      Handler3D::Mouse(view, button, x, y, pressed, button_state);
    }
  }

  void MouseMotion(pangolin::View& view, int x, int y, int button_state) {
    GetPosNormal(view, x, y, p, Pw, Pc, n);
    bool handled = false;
    for (std::map<int, SceneGraph::GLObject*>::iterator i =
             m_selected_objects.begin(); i != m_selected_objects.end(); ++i ) {
      handled |= i->second->MouseMotion(
          Eigen::Map<Eigen::Matrix<GLdouble, 3, 1> >(p).cast<double>(),
          Eigen::Map<Eigen::Matrix<GLdouble, 3, 1> >(Pw).cast<double>(),
          Eigen::Map<Eigen::Matrix<GLdouble, 3, 1> >(n).cast<double>(),
          button_state, i->first);
    }
    if (!handled) {
      pangolin::Handler3D::MouseMotion(view, x, y, button_state);
    }
  }

  void Special(pangolin::View& view, pangolin::InputSpecial inType,
               float x, float y, float p1, float p2, float p3, float p4,
               int button_state) {
    // TODO: Implement Special handler for GLObjects too

    GetPosNormal(view, x, y, p, Pw, Pc, n);

    bool handled = false;

    if (inType == pangolin::InputSpecialScroll) {
      m_selected_objects.clear();

      ComputeHits(view, *cam_state, x, y, m_grab_width, m_selected_objects);
      const pangolin::MouseButton button = p2 > 0 ?
          pangolin::MouseWheelUp : pangolin::MouseWheelDown;
      for (std::map<int, SceneGraph::GLObject*>::iterator i =
               m_selected_objects.begin();
           i != m_selected_objects.end(); ++i ) {
        handled |= i->second->Mouse(
            button,
            Eigen::Map<Eigen::Matrix<GLdouble, 3, 1>>(p).cast<double>(),
            Eigen::Map<Eigen::Matrix<GLdouble, 3, 1>>(Pw).cast<double>(),
            Eigen::Map<Eigen::Matrix<GLdouble, 3, 1>>(n).cast<double>(),
            true, button_state, i->first);
      }
    }

    if (!handled) {
      pangolin::Handler3D::Special(view, inType, x, y,
                                   p1, p2, p3, p4, button_state);
    }
  }

  std::map<int, SceneGraph::GLObject*> m_selected_objects;
  std::map<int, SceneGraph::GLObject*> m_selected_objects_;
  SceneGraph::GLSceneGraph& m_scenegraph;
  unsigned m_grab_width;
};

}  // namespace SceneGraph

#endif  // SCENEGRAPH_PANGOLINSCENEGRAPHHANDLER_H_
