#ifndef _GL_CVARS_H_
#define _GL_CVARS_H_

#include <CVars/CVar.h>

struct GLWindowConfig
{
    GLWindowConfig() : 
        m_bDebugLighting( CVarUtils::CreateCVar<>( "debug.Lighting", false, "Turn lighting debugging on") ),
        m_bShowMeshNormals( CVarUtils::CreateCVar<>( "debug.ShowMeshNormals", false, "Show mesh normals") ),
	m_bDebugSimCam( CVarUtils::CreateCVar<>( "debug.DebugSimCam", false, "Show a teapot for debugging") ),
	m_bDebugCaptureRGB( CVarUtils::CreateCVar<>( "debug.CaptureRGB", false, "Retrieve camera data (RPG) through pbo" ) ),
	m_bDebugCaptureDepth( CVarUtils::CreateCVar<>( "debug.CaptureDepth", false, "Retrieve camera data (Depth) through pbo" ) )

    {}

    bool&  m_bDebugLighting;
    bool&  m_bShowMeshNormals;
    bool&  m_bDebugSimCam;
    bool&  m_bDebugCaptureRGB;
    bool&  m_bDebugCaptureDepth;
};

extern GLWindowConfig gConfig;

#endif
