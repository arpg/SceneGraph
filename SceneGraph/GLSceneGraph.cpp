#include <SceneGraph/GLObject.h>
#include <SceneGraph/GLSceneGraph.h>

namespace SceneGraph
{

extern std::map<int,GLObject*>   g_mObjects; // map of id to objects

/////////////////////////////////////////////////////////////////////////////////
GLSceneGraph::GLSceneGraph()
    : GLObject("SceneGraph")
{
    Reset();
}

/////////////////////////////////////////////////////////////////////////////////
GLObject* GLSceneGraph::Root()
{
    return this;
}

/////////////////////////////////////////////////////////////////////////////////
void GLSceneGraph::Reset()
{
    // Remove children
    m_vpChildren.clear();

    // This object and children can be selected
    m_bIsSelectable = true;
}

/////////////////////////////////////////////////////////////////////////////////
void GLSceneGraph::DrawCanonicalObject()
{
    // Apply lights and any global properties
}

void GLSceneGraph::ApplyPreferredGlSettings()
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

    glLineWidth(1.5);
}


/////////////////////////////////////////////////////////////////////////////////
GLObject* GLSceneGraph::GetObject( unsigned int nId )
{
    std::map<int,GLObject*>::iterator it = g_mObjects.find( nId );
    if( it == g_mObjects.end() ){
        return NULL;
    }
    return it->second;
}

} // SceneGraph
