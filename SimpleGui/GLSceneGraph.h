#ifndef _GL_SCENE_GRAPH_H_
#define _GL_SCENE_GRAPH_H_

#include <SimpleGui/GLObject.h>
#include <map>

#include <SimpleGui/SceneGraphListener.h>

class GLSceneGraph : public GLObject
{
    public:
        GLSceneGraph();
        GLObject* Root();

        GLObject* GetObject( unsigned int nId );

        void RegisterObject( GLObject* pObj );

        void Reset();

        void draw();

        void ApplyDfsVisitor( void(*pVisitorFunc)(GLObject*) );

        void RegisterSceneGraphListener( SceneGraphListener *pSceneGraphListener );

    private:
        void _RecursiveDraw( GLObject* pObj );
        void _RecursiveVisit( void(*pVisitorFunc)(GLObject*), GLObject* pObj );

    private:
        std::vector<SceneGraphListener*> m_sceneGraphListeners;
};

#endif
