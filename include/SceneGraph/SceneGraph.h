#pragma once

#include <SceneGraph/GLHelpers.h>

// OpenGL defines are in here
#include <SceneGraph/AxisAlignedBoundingBox.h>
#include <SceneGraph/GLObject.h>
#include <SceneGraph/GLSceneGraph.h>
#include <SceneGraph/GLHeightmap.h>
#include <SceneGraph/GLGroup.h>
#include <SceneGraph/GLGrid.h>
#include <SceneGraph/GLAxis.h>
#include <SceneGraph/GLWireSphere.h>
#include <SceneGraph/GLLight.h>
#include <SceneGraph/simple_math.h>
#include <SceneGraph/gldraw.h>

#ifndef HAVE_GLES
    #include <SceneGraph/glvbo.h>

    #include <SceneGraph/GLCube.h>
    #include <SceneGraph/GLBox.h>
    #include <SceneGraph/GLAxisAlignedBox.h>
    #include <SceneGraph/GLMovableAxis.h>
    #include <SceneGraph/GLCylinder.h>

#ifdef HAVE_ASSIMP
    #include <SceneGraph/GLMesh.h>
#endif // HAVE_ASSIMP

#endif // HAVE_GLES

