#pragma once

#include <pangolin/pangolin.h>
#include <SceneGraph/SceneGraph.h>

struct ActivateDrawFunctor
{
    ActivateDrawFunctor(SceneGraph::GLObject& glObject, pangolin::OpenGlRenderState& renderState)
        :glObject(glObject), renderState(renderState)
    {
    }

    void operator()(pangolin::View& view) {
        view.Activate(renderState);
        glObject.DrawObjectAndChildren(GL_RENDER);
    }

    SceneGraph::GLObject& glObject;
    pangolin::OpenGlRenderState& renderState;
};

struct ActivateScissorClearDrawFunctor
{
    ActivateScissorClearDrawFunctor(SceneGraph::GLObject& glObject, pangolin::OpenGlRenderState& renderState)
        :glObject(glObject), renderState(renderState)
    {
    }

    void operator()(pangolin::View& view) {
        view.ActivateScissorAndClear(renderState);
        glObject.DrawObjectAndChildren(GL_RENDER);
    }

    SceneGraph::GLObject& glObject;
    pangolin::OpenGlRenderState& renderState;
};

struct ActivateScissorClearDrawFunctor3d2d
{
    ActivateScissorClearDrawFunctor3d2d(SceneGraph::GLObject& glObject3d, pangolin::OpenGlRenderState& renderState3d,
                                    SceneGraph::GLObject& glObject2d, pangolin::OpenGlRenderState& renderState2d)
        :glObject3d(glObject3d), renderState3d(renderState3d),
        glObject2d(glObject2d), renderState2d(renderState2d)
    {
    }

    void operator()(pangolin::View& view) {
        view.ActivateScissorAndClear(renderState3d);
        glObject3d.DrawObjectAndChildren(GL_RENDER);
        renderState2d.Apply();
        glObject2d.DrawObjectAndChildren(GL_RENDER);
    }

    SceneGraph::GLObject& glObject3d;
    pangolin::OpenGlRenderState& renderState3d;
    SceneGraph::GLObject& glObject2d;
    pangolin::OpenGlRenderState& renderState2d;
};

struct HandlerSceneGraph : pangolin::Handler3D
{

    HandlerSceneGraph(SceneGraph::GLSceneGraph& graph, pangolin::OpenGlRenderState& cam_state, pangolin::AxisDirection enforce_up=pangolin::AxisNone, float trans_scale=0.01f)
        : pangolin::Handler3D(cam_state,enforce_up, trans_scale), m_scenegraph(graph), m_grab_width(15) {}

    void ProcessHitBuffer (GLint hits, GLuint* buf, std::map<int,SceneGraph::GLObject*>& objects )
    {
        GLuint* closestNames = 0;
        GLuint closestNumNames;
        GLuint closestZ = std::numeric_limits<GLuint>::max();

        for (int i = 0; i < hits; i++) {
            if (buf[1] < closestZ) {
                closestNames = buf+3;
                closestNumNames = buf[0];
                closestZ = buf[1];
            }
            buf += buf[0]+3;
        }
        for (unsigned int i = 0; i < closestNumNames; i++) {
            const int pickId = closestNames[i];
            SceneGraph::GLObject* obj = m_scenegraph.GetObject(pickId);
            if(obj) {
                objects[pickId] = obj;
            }
        }
    }

    void ComputeHits(pangolin::View& display, const pangolin::OpenGlRenderState& cam_state, int x, int y, int grab_width, std::map<int,SceneGraph::GLObject*>& hit_objects )
    {
        // Get views viewport / modelview /projection
        GLint viewport[4] = {display.v.l,display.v.b,display.v.w,display.v.h};
        pangolin::OpenGlMatrix mv = cam_state.GetModelViewMatrix();
        pangolin::OpenGlMatrix proj = cam_state.GetProjectionMatrix();

        // Prepare hit buffer object
        const unsigned int MAX_SEL_SIZE = 64;
        GLuint vSelectBuf[MAX_SEL_SIZE];
        glSelectBuffer( MAX_SEL_SIZE, vSelectBuf );

        // Load and adjust modelview projection matrices
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPickMatrix( x, y, grab_width, grab_width, viewport );
        proj.Multiply();
        glMatrixMode(GL_MODELVIEW);
        mv.Load();

        // Render scenegraph in 'select' mode
        glRenderMode( GL_SELECT );
        glInitNames();
        m_scenegraph.DrawObjectAndChildren(GL_SELECT);
        glFlush();

        GLint nHits = glRenderMode( GL_RENDER );
        if( nHits > 0 ){
            ProcessHitBuffer( nHits, vSelectBuf, hit_objects );
        }
    }

    void GetPosNormal(pangolin::View& view, int x, int y, Eigen::Vector3d& p, Eigen::Vector3d& P, Eigen::Vector3d& n)
    {
        const GLint viewport[4] = {view.v.l,view.v.b,view.v.w,view.v.h};
        const pangolin::OpenGlMatrix proj = cam_state->GetProjectionMatrix();
        const pangolin::OpenGlMatrix mv = cam_state->GetModelViewMatrix();

        glReadBuffer(GL_FRONT);
        const int zl = (hwin*2+1);
        const int zsize = zl*zl;
        GLfloat zs[zsize];
        glReadPixels(x-hwin,y-hwin,zl,zl,GL_DEPTH_COMPONENT,GL_FLOAT,zs);
        const GLfloat mindepth = *(std::min_element(zs,zs+zsize));

        p << x, y, mindepth;
        gluUnProject(x, y, mindepth, mv.m, proj.m, viewport, &P(0), &P(1), &P(2));

        Eigen::Vector3d Pl,Pr,Pb,Pt;
        gluUnProject(x-hwin, y, zs[hwin*zl + 0],    mv.m, proj.m, viewport, &Pl(0), &Pl(1), &Pl(2));
        gluUnProject(x+hwin, y, zs[hwin*zl + zl-1], mv.m, proj.m, viewport, &Pr(0), &Pr(1), &Pr(2));
        gluUnProject(x, y-hwin, zs[hwin+1],         mv.m, proj.m, viewport, &Pb(0), &Pb(1), &Pb(2));
        gluUnProject(x, y+hwin, zs[zsize-(hwin+1)], mv.m, proj.m, viewport, &Pt(0), &Pt(1), &Pt(2));

        n = ((Pr-Pl).cross(Pt-Pb)).normalized();
    }

    void Mouse(pangolin::View& view, pangolin::MouseButton button, int x, int y, bool pressed, int button_state)
    {
        Eigen::Vector3d p, P, n;
        GetPosNormal(view,x,y,p,P,n);

        bool handled = false;

        if(pressed) {
            m_selected_objects.clear();
            ComputeHits(view,*cam_state,x,y,m_grab_width,m_selected_objects);
            for(std::map<int,SceneGraph::GLObject*>::iterator i = m_selected_objects.begin(); i != m_selected_objects.end(); ++i ) {
                handled |= i->second->Mouse(button, p, P, n, pressed, button_state, i->first);
            }
        }else{
            for(std::map<int,SceneGraph::GLObject*>::iterator i = m_selected_objects.begin(); i != m_selected_objects.end(); ++i ) {
                handled |= i->second->Mouse(button, p, P, n, pressed, button_state, i->first);
            }
        }
        if(!handled) {
            Handler3D::Mouse(view,button,x,y,pressed,button_state);
        }
    }

    void MouseMotion(pangolin::View& view, int x, int y, int button_state)
    {
        Eigen::Vector3d p, P, n;
        GetPosNormal(view,x,y,p,P,n);

        bool handled = false;

        for(std::map<int,SceneGraph::GLObject*>::iterator i = m_selected_objects.begin(); i != m_selected_objects.end(); ++i ) {
            handled |= i->second->MouseMotion(p, P, n, button_state, i->first);
        }

        if(!handled) {
            pangolin::Handler3D::MouseMotion(view,x,y,button_state);
        }
    }

    std::map<int,SceneGraph::GLObject*> m_selected_objects;
    SceneGraph::GLSceneGraph& m_scenegraph;
    unsigned m_grab_width;
};
