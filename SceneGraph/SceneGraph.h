#pragma once

#include <SceneGraph/config.h>

//opengl defines are in here
#include <SceneGraph/GLHelpers.h>
#include <SceneGraph/GLObject.h>
#include <SceneGraph/GLSceneGraph.h>
#include <SceneGraph/AxisAlignedBoundingBox.h>
#include <SceneGraph/GLLight.h>

#ifndef _ANDROID_
#include <SceneGraph/GLHelpers.h>




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
#include <SceneGraph/GLCylinder.h>

#ifdef HAVE_ASSIMP
#include <SceneGraph/GLMesh.h>
#endif // HAVE_ASSIMP

#ifdef HAVE_PANGOLIN
#include <SceneGraph/Pangolin.h>
#endif // HAVE_PANGOLIN

#endif // _ANDROID
