#ifndef _GL_SCENE_GRAPH_H_
#define _GL_SCENE_GRAPH_H_

#include <SceneGraph/GLObject.h>
#include <SceneGraph/GLLight.h>
#include <map>

namespace SceneGraph
{

class GLSceneGraph : public GLObject
{
    public:
        GLSceneGraph();

        GLObject* Root();
        GLObject* GetObject( unsigned int nId );

        // Remove everything
        void Clear();

        // Reset to default state
        void Reset();

        // Add light to scene
        GLLight& AddLight(Eigen::Vector3d pos);

        // return ith light added by user
        GLLight& GetLight(unsigned int i);

        // Show / Hide rendering of lights
        void ShowLights(bool showLights = true);

        // Show / Hide rendering of shadows
        void ShowShaddows(bool showShaddows = true);

        // Perform any scene setup
        void DrawCanonicalObject();

        // Override GLObject method to provide specific drawing methods
        void DrawObjectAndChildren(RenderMode renderMode = eRenderVisible);

        static void ApplyPreferredGlSettings();

    private:
        // These lights are owned by the SceneGraph (it takes care of destruction)
        std::vector<GLLight*>    m_vpLights;

        bool m_bShowLights;
        bool m_bShowShaddows;
};

}

#endif
