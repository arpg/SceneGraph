#pragma once

#include <pangolin/pangolin.h>
#include <SceneGraph/SceneGraph.h>

namespace SceneGraph
{

struct ActivateDrawFunctor
{
    ActivateDrawFunctor(SceneGraph::GLObject& glObject, pangolin::OpenGlRenderState& renderState)
        :glObject(glObject), renderState(renderState)
    {
    }

    void operator()(pangolin::View& view) {
        view.Activate(renderState);
        glObject.DrawObjectAndChildren( eRenderVisible );
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
        glObject.DrawObjectAndChildren(eRenderVisible);
    }

    SceneGraph::GLObject& glObject;
    pangolin::OpenGlRenderState& renderState;
};

struct ActivateDrawFunctor3d2d
{
    ActivateDrawFunctor3d2d(SceneGraph::GLObject& glObject3d, pangolin::OpenGlRenderState& renderState3d,
                                    SceneGraph::GLObject& glObject2d, pangolin::OpenGlRenderState& renderState2d)
        :glObject3d(glObject3d), renderState3d(renderState3d),
        glObject2d(glObject2d), renderState2d(renderState2d)
    {
    }

    void operator()(pangolin::View& view) {
        view.Activate(renderState3d);
        glObject3d.DrawObjectAndChildren(eRenderVisible);
        renderState2d.Apply();
        glObject2d.DrawObjectAndChildren(eRenderVisible);
    }

    SceneGraph::GLObject& glObject3d;
    pangolin::OpenGlRenderState& renderState3d;
    SceneGraph::GLObject& glObject2d;
    pangolin::OpenGlRenderState& renderState2d;
};

struct ActivateScissorClearDrawFunctor3d2d
{
    ActivateScissorClearDrawFunctor3d2d(SceneGraph::GLObject& glObject3d, pangolin::OpenGlRenderState& renderState3d,
                                    SceneGraph::GLObject& glObject2d, pangolin::OpenGlRenderState& renderState2d)
        :glObject3d(glObject3d), renderState3d(renderState3d),
        glObject2d(glObject2d), renderState2d(renderState2d)
    {
    }

    void operator()(pangolin::View& view) {
        view.ActivateScissorAndClear(renderState3d);
        glObject3d.DrawObjectAndChildren(eRenderVisible);
        renderState2d.Apply();
        glObject2d.DrawObjectAndChildren(eRenderVisible);
    }

    SceneGraph::GLObject& glObject3d;
    pangolin::OpenGlRenderState& renderState3d;
    SceneGraph::GLObject& glObject2d;
    pangolin::OpenGlRenderState& renderState2d;
};

}
