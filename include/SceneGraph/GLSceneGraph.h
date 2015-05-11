#ifndef _GL_SCENE_GRAPH_H_
#define _GL_SCENE_GRAPH_H_

#include <SceneGraph/GLObject.h>
#include <map>

namespace SceneGraph
{

class GLObjectPrePostRender;

class SCENEGRAPH_EXPORT GLSceneGraph : public GLObject
{
    public:
        GLSceneGraph();

        GLObject* Root();
        GLObject* GetObject( unsigned int nId );

        // Remove everything
        void Clear();

        // Reset to default state
        void Reset();

        // Add GLObject to scenegraph
        void AddChild( GLObject* pChild );
        bool RemoveChild( GLObject* pChild );

        void DrawCanonicalObject();

        void DrawObjectAndChildren(RenderMode renderMode = eRenderVisible);
        static void ApplyPreferredGlSettings();

    private:
        void PreDraw();
        void PostDraw();

        // These lights are owned by the SceneGraph (it takes care of destruction)
        std::vector<GLObjectPrePostRender*> m_vpPrePostRender;

        bool m_bEnableLighting;

};

class SCENEGRAPH_EXPORT GLObjectPrePostRender : public GLObject
{
public:
    GLObjectPrePostRender() {}

    GLObjectPrePostRender(std::string name)
        : GLObject(name)
    {
    }

    virtual void PreRender(GLSceneGraph& scene) = 0;

    virtual void PostRender(GLSceneGraph& scene) = 0;
};

}

#endif
