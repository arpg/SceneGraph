#pragma once

#include <pangolin/pangolin.h>

#ifdef _ANDROID_
#include <SceneGraph/GLES_compat.h>
#endif

#ifdef _MSVC_
// MSVC doesn't define  C99's snprintf directly. _snprintf_s is equivelent.
#define snprintf _snprintf_s
#endif

#ifdef _MSVC_
   // MSVC requires export annotations in order to generate DLL's
#  include <SceneGraph/scenegraph_export.h>
#else
#  define SCENEGRAPH_EXPORT
#endif
