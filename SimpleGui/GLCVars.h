#ifndef _GL_CVARS_H_
#define _GL_CVARS_H_

#include <CVars/CVar.h>

struct GLWindowConfig
{
    GLWindowConfig() : 
        m_bDebugLighting( CVarUtils::CreateCVar<>( "debug.Lighting", false, "Turn lighting debugging on") ),
        m_bShowMeshNormals( CVarUtils::CreateCVar<>( "debug.ShowMeshNormals", false, "Show mesh normals") ),
        m_bDebugSimCam( CVarUtils::CreateCVar<>( "debug.DebugSimCam", false, "Show a teapot for debugging") ) 
    {}

    bool&  m_bDebugLighting;
    bool&  m_bShowMeshNormals;
    bool&  m_bDebugSimCam;
};

extern GLWindowConfig gConfig;

#endif
