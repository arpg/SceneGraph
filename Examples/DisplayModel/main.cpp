#include <iostream>
#include <pangolin/pangolin.h>
#include <SceneGraph/GLSceneGraph.h>
#include <SceneGraph/GLMesh.h>
#include <SceneGraph/GLGrid.h>
#include <SceneGraph/GLWaypoint.h>
#include <SceneGraph/GLLineStrip.h>

#include <boost/bind.hpp>

#include <Eigen/Eigen>


using namespace SceneGraph;
using namespace pangolin;
using namespace std;

struct ActivateDrawFunctor
{
    ActivateDrawFunctor(SceneGraph::GLObject& glObject, pangolin::OpenGlRenderState& renderState)
        :glObject(glObject), renderState(renderState)
    {
    }

    void operator()(pangolin::View& view) {
        view.Activate(renderState);
        glObject.DrawObjectAndChildren();
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
        glObject.DrawObjectAndChildren();
    }

    SceneGraph::GLObject& glObject;
    pangolin::OpenGlRenderState& renderState;
};

struct HandlerSceneGraph : Handler3D
{

    HandlerSceneGraph(GLSceneGraph& graph, OpenGlRenderState& cam_state, AxisDirection enforce_up=AxisNone, float trans_scale=0.01f)
        : Handler3D(cam_state,enforce_up, trans_scale), m_scenegraph(graph), m_grab_width(15) {}

    void ProcessHitBuffer (GLint hits, GLuint* buf, std::map<int,GLObject*>& objects )
    {
        GLuint* closestNames = 0;
        GLuint closestNumNames;
        GLuint closestZ = numeric_limits<GLuint>::max();

        for (int i = 0; i < hits; i++) {
            if (buf[1] < closestZ) {
                closestNames = buf+3;
                closestNumNames = buf[0];
                closestZ = buf[1];
            }
            buf += buf[0]+3;
        }
//        cout << "------" << endl;
        for (unsigned int i = 0; i < closestNumNames; i++) {
            const int pickId = closestNames[i];
            GLObject* obj = m_scenegraph.GetObject(pickId);
            if(obj) {
                objects[pickId] = obj;
//                cout << obj->ObjectName();
            }
//            cout << " (" << pickId << "), ";
        }
//        cout << endl;
    }

    void ComputeHits(View& display, const OpenGlRenderState& cam_state, int x, int y, int grab_width, std::map<int,GLObject*>& hit_objects )
    {
        // Get views viewport / modelview /projection
        GLint viewport[4] = {display.v.l,display.v.b,display.v.w,display.v.h};
        OpenGlMatrix mv = cam_state.GetModelViewMatrix();
        OpenGlMatrix proj = cam_state.GetProjectionMatrix();

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
        m_scenegraph.DrawObjectAndChildren();
        glFlush();

        GLint nHits = glRenderMode( GL_RENDER );
        if( nHits > 0 ){
            ProcessHitBuffer( nHits, vSelectBuf, hit_objects );
        }
    }

    void GetPosNormal(View& view, int x, int y, Eigen::Vector3d& p, Eigen::Vector3d& P, Eigen::Vector3d& n)
    {
        const GLint viewport[4] = {view.v.l,view.v.b,view.v.w,view.v.h};
        const OpenGlMatrix proj = cam_state->GetProjectionMatrix();
        const OpenGlMatrix mv = cam_state->GetModelViewMatrix();

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

    void Mouse(View& view, MouseButton button, int x, int y, bool pressed, int button_state)
    {
        Eigen::Vector3d p, P, n;
        GetPosNormal(view,x,y,p,P,n);

        bool handled = false;

        if(pressed) {
            m_selected_objects.clear();
            ComputeHits(view,*cam_state,x,y,m_grab_width,m_selected_objects);
            for(std::map<int,GLObject*>::iterator i = m_selected_objects.begin(); i != m_selected_objects.end(); ++i ) {
                handled |= i->second->Mouse(button, p, P, n, pressed, button_state, i->first);
            }
        }else{
            for(std::map<int,GLObject*>::iterator i = m_selected_objects.begin(); i != m_selected_objects.end(); ++i ) {
                handled |= i->second->Mouse(button, p, P, n, pressed, button_state, i->first);
            }
        }
        if(!handled) {
            Handler3D::Mouse(view,button,x,y,pressed,button_state);
        }
    }

    void MouseMotion(View& view, int x, int y, int button_state)
    {
        Eigen::Vector3d p, P, n;
        GetPosNormal(view,x,y,p,P,n);

        bool handled = false;

        for(std::map<int,GLObject*>::iterator i = m_selected_objects.begin(); i != m_selected_objects.end(); ++i ) {
            handled |= i->second->MouseMotion(p, P, n, button_state, i->first);
        }

        if(!handled) {
            Handler3D::MouseMotion(view,x,y,button_state);
        }
    }

    std::map<int,GLObject*> m_selected_objects;
    GLSceneGraph& m_scenegraph;
    unsigned m_grab_width;
};

void SetupOpenGL()
{
    glShadeModel(GL_SMOOTH);

    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    GLfloat light_pos[] = {0,0,0.01};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos );

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_NORMALIZE);

    glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
    glEnable( GL_COLOR_MATERIAL );

    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
    glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );

    glDepthFunc( GL_LEQUAL );
    glEnable( GL_DEPTH_TEST );

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main( int /*argc*/, char** /*argv[]*/ )
{  
    // Create OpenGL window in single line thanks to GLUT
    pangolin::CreateGlutWindowAndBind("Main",640,480);
    SetupOpenGL();

    // Define objects to draw
    GLGrid glGrid(50,1.0);
    glGrid.SetPerceptable(true);
    GLMesh glCar("beatle-no-wheels-no-interior-embedded-texture.blend");
    GLWayPoint glWaypoint;

    // Add to scenegraph
    GLSceneGraph glGraph;
    glGraph.AddChild(&glGrid);
//    glGraph.AddChild(&glCar);
    glGraph.AddChild(&glWaypoint);

    // Define Camera Render Object (for view / scene browsing)
    pangolin::OpenGlRenderState renderState(
                ProjectionMatrix(640,480,420,420,320,240,0.1,1000),
                Pose(0,0,-3, AxisNegZ, AxisY)
                );

    // Add viewport to window and provide 3D Handler
    View& v3d = pangolin::CreateDisplay()
            .SetBounds(0.0, 1.0, 0.0, 1.0, -640.0f/480.0f)
            .SetHandler(new HandlerSceneGraph(glGraph,renderState,AxisZ))
            .SetDrawFunction(ActivateScissorClearDrawFunctor(glGraph, renderState));

    // Default hooks for exiting (Esc) and fullscreen (tab).
    while( !pangolin::ShouldQuit() )
    {
        v3d.ActivateScissorAndClear(renderState);

        // Swap frames and Process Events
        FinishGlutFrame();

        usleep(1000);
    }

    return 0;
}
