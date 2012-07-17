#include <SceneGraph/GLObject.h>
#include <SceneGraph/GLSceneGraph.h>

namespace SceneGraph
{

extern std::map<int,GLObject*>   g_mObjects; // map of id to objects

/////////////////////////////////////////////////////////////////////////////////
GLSceneGraph::GLSceneGraph()
    : GLObject("SceneGraph")
{
    m_vpChildren.clear();
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

    // Remove id->child mappings
    g_mObjects.clear();
}

/////////////////////////////////////////////////////////////////////////////////
void GLSceneGraph::DrawCanonicalObject()
{
    // Apply lights and any global properties
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
