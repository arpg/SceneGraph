#pragma once

#include <pangolin/pangolin.h>
#include <SceneGraph/SceneGraph.h>

namespace SceneGraph
{

struct HandlerSceneGraph : pangolin::Handler3D
{
    HandlerSceneGraph(SceneGraph::GLSceneGraph& graph, pangolin::OpenGlRenderState& cam_state, pangolin::AxisDirection enforce_up=pangolin::AxisNone, float trans_scale=0.01f)
        : pangolin::Handler3D(cam_state,enforce_up, trans_scale), m_scenegraph(graph), m_grab_width(15) {}

    void ProcessHitBuffer (GLint hits, GLuint* buf, std::map<int,SceneGraph::GLObject*>& objects )
    {
        GLuint* closestNames = 0;
        GLuint closestNumNames = 0;
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
#ifndef HAVE_GLES
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
        m_scenegraph.DrawObjectAndChildren(eRenderSelectable);
        glFlush();

        GLint nHits = glRenderMode( eRenderVisible );
        if( nHits > 0 ){
            ProcessHitBuffer( nHits, vSelectBuf, hit_objects );
        }
#endif // HAVE_GLES
    }

    void Mouse(pangolin::View& view, pangolin::MouseButton button, int x, int y, bool pressed, int button_state)
    {
        GetPosNormal(view,x,y,p,Pw,Pc,n);

        bool handled = false;

        if(pressed) {
            m_selected_objects.clear();
            ComputeHits(view,*cam_state,x,y,m_grab_width,m_selected_objects);
            for(std::map<int,SceneGraph::GLObject*>::iterator i = m_selected_objects.begin(); i != m_selected_objects.end(); ++i ) {
                handled |= i->second->Mouse( button,
                    Eigen::Map<Eigen::Matrix<GLdouble,3,1> >(p).cast<double>(),
                    Eigen::Map<Eigen::Matrix<GLdouble,3,1> >(Pw).cast<double>(),
                    Eigen::Map<Eigen::Matrix<GLdouble,3,1> >(n).cast<double>(),
                    pressed, button_state, i->first
                );
            }
        }else{
            for(std::map<int,SceneGraph::GLObject*>::iterator i = m_selected_objects.begin(); i != m_selected_objects.end(); ++i ) {
                handled |= i->second->Mouse(button,
                    Eigen::Map<Eigen::Matrix<GLdouble,3,1> >(p).cast<double>(),
                    Eigen::Map<Eigen::Matrix<GLdouble,3,1> >(Pw).cast<double>(),
                    Eigen::Map<Eigen::Matrix<GLdouble,3,1> >(n).cast<double>(),
                    pressed, button_state, i->first
                );
            }
        }
        if(!handled) {
            Handler3D::Mouse(view,button,x,y,pressed,button_state);
        }
    }

    void MouseMotion(pangolin::View& view, int x, int y, int button_state)
    {
        GetPosNormal(view,x,y,p,Pw,Pc,n);

        bool handled = false;

        for(std::map<int,SceneGraph::GLObject*>::iterator i = m_selected_objects.begin(); i != m_selected_objects.end(); ++i ) {
            handled |= i->second->MouseMotion(
                Eigen::Map<Eigen::Matrix<GLdouble,3,1> >(p).cast<double>(),
                Eigen::Map<Eigen::Matrix<GLdouble,3,1> >(Pw).cast<double>(),
                Eigen::Map<Eigen::Matrix<GLdouble,3,1> >(n).cast<double>(),
                button_state, i->first
            );
        }

        if(!handled) {
            pangolin::Handler3D::MouseMotion(view,x,y,button_state);
        }
    }

    void Special(pangolin::View& view, pangolin::InputSpecial inType, float x, float y, float p1, float p2, float p3, float p4, int button_state)
    {
        // TODO: Implement Special handler for GLObjects too

        GetPosNormal(view,x,y,p,Pw,Pc,n);

        bool handled = false;

        if(inType == pangolin::InputSpecialScroll) {
            m_selected_objects.clear();
            ComputeHits(view,*cam_state,x,y,m_grab_width,m_selected_objects);

            const pangolin::MouseButton button = p2 > 0 ? pangolin::MouseWheelUp : pangolin::MouseWheelDown;
            for(std::map<int,SceneGraph::GLObject*>::iterator i = m_selected_objects.begin(); i != m_selected_objects.end(); ++i ) {
                handled |= i->second->Mouse(button,
                    Eigen::Map<Eigen::Matrix<GLdouble,3,1> >(p).cast<double>(),
                    Eigen::Map<Eigen::Matrix<GLdouble,3,1> >(Pw).cast<double>(),
                    Eigen::Map<Eigen::Matrix<GLdouble,3,1> >(n).cast<double>(),
                    true, button_state, i->first
                );
            }
        }

        if(!handled) {
            pangolin::Handler3D::Special(view,inType,x,y,p1,p2,p3,p4,button_state);
        }
    }


    std::map<int,SceneGraph::GLObject*> m_selected_objects;
    SceneGraph::GLSceneGraph& m_scenegraph;
    unsigned m_grab_width;
};

}
