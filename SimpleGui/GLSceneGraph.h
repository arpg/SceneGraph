#ifndef _GL_SCENE_GRAPH_H_
#define _GL_SCENE_GRAPH_H_

#include <SimpleGui/GLObject.h>
#include <map>

class GLSceneGraph : public GLObject
{
    public:
        GLObject* Root();

        GLObject* GetObject( unsigned int nId );

        void RegisterObject( GLObject* pObj );

        void Reset();

        void draw();

    private:
        void _RecursiveDraw( GLObject* pObj );

    private:

};

#endif
