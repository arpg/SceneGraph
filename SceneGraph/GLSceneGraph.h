#ifndef _GL_SCENE_GRAPH_H_
#define _GL_SCENE_GRAPH_H_

#include <SceneGraph/GLObject.h>
#include <map>

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

    private:
        void _RecursiveDraw( GLObject* pObj );
        void _RecursiveVisit( void(*pVisitorFunc)(GLObject*), GLObject* pObj );
};

#endif
