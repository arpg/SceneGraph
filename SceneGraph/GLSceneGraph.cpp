#include <SceneGraph/GLObject.h>
#include <SceneGraph/GLSceneGraph.h>

extern std::map<int,GLObject*>   g_mObjects; // map of id to objects

/////////////////////////////////////////////////////////////////////////////////
GLSceneGraph::GLSceneGraph()
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
    m_vpChildren.clear(); // HACK -- should recursively clear these guys...
    g_mObjects.clear();
}

/////////////////////////////////////////////////////////////////////////////////
void GLSceneGraph::draw() 
{

    // try to draw all children
    for( size_t ii = 0; ii < m_vpChildren.size(); ii++ ){
        GLObject* pObj = m_vpChildren[ii];
        //printf("%p : ", pObj );
        //fflush(stdout);
        //printf("m_vpChildren[%d], %s, has %d children\n", (int)ii, pObj->m_sObjectName.c_str(), (int)pObj->m_vpChildren.size() );
        _RecursiveDraw( pObj  );
    }

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

/////////////////////////////////////////////////////////////////////////////////
void GLSceneGraph::_RecursiveDraw( GLObject* pObj )
{
    //printf("Drawing %p %s\n", pObj, pObj->ObjectName() );
    if( pObj->IsVisible() ){
        pObj->draw();
    }else{
//        printf("NOT Drawing %s\n", pObj->ObjectName() );
    }

    // try to draw all children
    for( size_t ii = 0; ii < pObj->m_vpChildren.size(); ii++ ){
        _RecursiveDraw( pObj->m_vpChildren[ii] );
    }
}

/////////////////////////////////////////////////////////////////////////////////
void GLSceneGraph::_RecursiveVisit( void(*pVisitorFunc)(GLObject*), GLObject* pObj )
{
    (*pVisitorFunc)( pObj );
    // try to draw all children
    for( size_t ii = 0; ii < pObj->m_vpChildren.size(); ii++ ){
        _RecursiveVisit( pVisitorFunc, pObj->m_vpChildren[ii] );
    }
}

/////////////////////////////////////////////////////////////////////////////////
void GLSceneGraph::ApplyDfsVisitor( void(*pVisitorFunc)(GLObject*) )
{
    // try to draw all children
    for( size_t ii = 0; ii < m_vpChildren.size(); ii++ ){
        _RecursiveVisit( pVisitorFunc, m_vpChildren[ii] );
    }
}

