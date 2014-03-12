#pragma once

#include <pangolin/pangolin.h>

#ifdef _ANDROID_
#include <SceneGraph/GLES_compat.h>
#endif

#ifdef _MSVC_
// MSVC doesn't define  C99's snprintf directly. _snprintf_s is equivelent.
#define snprintf _snprintf_s
#endif