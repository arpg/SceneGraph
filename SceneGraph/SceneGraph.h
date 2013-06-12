#pragma once

#include <SceneGraph/config.h>

// OpenGL defines are in here
#include <SceneGraph/GLHelpers.h>
#include <SceneGraph/GLObject.h>
#include <SceneGraph/GLSceneGraph.h>
#include <SceneGraph/GLGroup.h>
#include <SceneGraph/AxisAlignedBoundingBox.h>
#include <SceneGraph/GLLight.h>

#ifdef HAVE_PANGOLIN
    #include <SceneGraph/PangolinDrawGLObject.h>
    #include <SceneGraph/PangolinSceneGraphHandler.h>
#endif // HAVE_PANGOLIN

#ifndef HAVE_GLES
    #include <SceneGraph/GLGrid.h>
    #include <SceneGraph/GLAxis.h>
    #include <SceneGraph/GLCube.h>
    #include <SceneGraph/GLBox.h>
    #include <SceneGraph/GLOpenBox.h>
    #include <SceneGraph/GLAxisAlignedBox.h>
    #include <SceneGraph/GLMovableAxis.h>
    #include <SceneGraph/GLWaypoint.h>
    #include <SceneGraph/GLLineStrip.h>
    #include <SceneGraph/GLCylinder.h>
    
    #ifdef HAVE_ASSIMP
        #include <SceneGraph/GLMesh.h>
    #endif // HAVE_ASSIMP

    #ifdef HAVE_PANGOLIN
        #include <SceneGraph/PangolinImageView.h>
        #include <SceneGraph/PangolinGlCachedSizeableBuffer.h>
        #include <SceneGraph/GLShadowLight.h>
        #include <SceneGraph/GLVbo.h>
        #include <SceneGraph/GLPrimitives.h>
        #include <SceneGraph/GLAxisHistory.h>
        #include <SceneGraph/GLText.h>
    #endif // HAVE_PANGOLIN
#endif // HAVE_GLES
