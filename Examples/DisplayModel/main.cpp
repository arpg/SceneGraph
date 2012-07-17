#include <iostream>
#include <pangolin/pangolin.h>
#include <SceneGraph/GLSceneGraph.h>
#include <SceneGraph/GLMesh.h>
#include <SceneGraph/GLGrid.h>
#include <boost/bind.hpp>


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
        : Handler3D(cam_state,enforce_up, trans_scale), m_scenegraph(graph), m_grab_width(10) {}

    void ProcessHitBuffer (GLint hits, GLuint* buf, std::vector<GLObject*>& objects )
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
        for (unsigned int i = 0; i < closestNumNames; i++) {
            GLObject* obj = m_scenegraph.GetObject(closestNames[i]);
            if(obj) objects.push_back(obj);
        }
    }

    void ComputeHits(View& display, const OpenGlRenderState& cam_state, int x, int y, int grab_width, std::vector<GLObject*>& hit_objects )
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

    void Mouse(View& display, MouseButton button, int x, int y, bool pressed, int button_state)
    {
        if(pressed) {
            m_selected_objects.clear();
            ComputeHits(display,*cam_state,x,y,m_grab_width,m_selected_objects);
            BOOST_FOREACH(GLObject* obj, m_selected_objects) {
                obj->Mouse(button,x, y, pressed, button_state);
            }
        }else{
            BOOST_FOREACH(GLObject* obj, m_selected_objects) {
                obj->Mouse(button,x, y, pressed, button_state);
            }
        }
        Handler3D::Mouse(display,button,x,y,pressed,button_state);
    }

    void MouseMotion(View& view, int x, int y, int button_state)
    {
        BOOST_FOREACH(GLObject* obj, m_selected_objects) {
            obj->MouseMotion(x, y, button_state);
        }
        Handler3D::MouseMotion(view,x,y,button_state);
    }

    std::vector<GLObject*> m_selected_objects;
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
    GLMesh glCar("beatle-no-wheels-no-interior-embedded-texture.blend");
    GLGrid glGrid;

    // Add to scenegraph
    GLSceneGraph glGraph;
    glGraph.AddChild(&glCar);
    glGraph.AddChild(&glGrid);

    // Define Camera Render Object (for view / scene browsing)
    pangolin::OpenGlRenderState renderState(
                ProjectionMatrix(640,480,420,420,320,240,0.1,1000),
                Pose(0,0,-3, AxisNegZ, AxisY)
                );

    // Add viewport to window and provide 3D Handler
    View& v3d = pangolin::CreateDisplay()
            .SetBounds(0.0, 1.0, 0.0, 1.0, -640.0f/480.0f)
            .SetHandler(new HandlerSceneGraph(glGraph,renderState,AxisNegZ));
    v3d.extern_draw_function = ActivateScissorClearDrawFunctor(glGraph, renderState);

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
