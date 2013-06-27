#pragma once

#include <SceneGraph/config.h>

// OpenGL defines are in here
#include <pangolin/pangolin.h>
#include <pangolin/gldraw.h>

#include <SceneGraph/AxisAlignedBoundingBox.h>
#include <SceneGraph/GLHelpers.h>
#include <SceneGraph/GLObject.h>
#include <SceneGraph/GLSceneGraph.h>
#include <SceneGraph/GLGroup.h>
#include <SceneGraph/GLGrid.h>
#include <SceneGraph/GLAxis.h>
#include <SceneGraph/GLLight.h>

// GLObjects that use pangolin
#include <SceneGraph/GLLineStrip.h>
#include <SceneGraph/GLPrimitives.h>

// Objects that could be refactored into pangolin
#include <SceneGraph/PangolinDrawGLObject.h>
#include <SceneGraph/PangolinSceneGraphHandler.h>
#include <SceneGraph/PangolinGlCachedSizeableBuffer.h>
#include <SceneGraph/PangolinImageView.h>

#ifndef HAVE_GLES
    #include <SceneGraph/GLCube.h>
    #include <SceneGraph/GLBox.h>
    #include <SceneGraph/GLOpenBox.h>
    #include <SceneGraph/GLAxisAlignedBox.h>
    #include <SceneGraph/GLMovableAxis.h>
    #include <SceneGraph/GLWaypoint.h>
    #include <SceneGraph/GLCylinder.h>
    #include <SceneGraph/GLShadowLight.h>
    #include <SceneGraph/GLVbo.h>
    #include <SceneGraph/GLAxisHistory.h>
    #include <SceneGraph/GLText.h>

#ifdef HAVE_ASSIMP
    #include <SceneGraph/GLMesh.h>
#endif // HAVE_ASSIMP

#endif // HAVE_GLES
