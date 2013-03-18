#pragma once

#include <SceneGraph/config.h>

#include <SceneGraph/GLSceneGraph.h>
#include <SceneGraph/GLObject.h>
#include <SceneGraph/GLGroup.h>

#include <SceneGraph/GLGrid.h>
#include <SceneGraph/GLAxis.h>
#include <SceneGraph/GLCube.h>
#include <SceneGraph/GLBox.h>
#include <SceneGraph/GLOpenBox.h>
#include <SceneGraph/GLAxisAlignedBox.h>
#include <SceneGraph/GLMovableAxis.h>
#include <SceneGraph/GLWaypoint.h>
#include <SceneGraph/GLLineStrip.h>

#ifdef HAVE_ASSIMP
#include <SceneGraph/GLMesh.h>
#endif // HAVE_ASSIMP

#ifdef HAVE_PANGOLIN
#include <SceneGraph/Pangolin.h>
#endif // HAVE_PANGOLIN
