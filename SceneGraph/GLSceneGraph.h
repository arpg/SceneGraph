#ifndef _GL_SCENE_GRAPH_H_
#define _GL_SCENE_GRAPH_H_

#include <SceneGraph/GLObject.h>
#include <map>

namespace SceneGraph
{

class GLSceneGraph : public GLObject
{
    public:
        GLSceneGraph();

        GLObject* Root();
        GLObject* GetObject( unsigned int nId );

        void Reset();

        // Perform any scene setup
        void DrawCanonicalObject();

        static void ApplyPreferredGlSettings();

    private:
};

}

#endif
