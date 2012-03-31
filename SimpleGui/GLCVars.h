#ifndef _GL_CVARS_H_
#define _GL_CVARS_H_

#include <CVars/CVar.h>

struct GLWindowConfig
{
    GLWindowConfig() : 
        m_bDebugLighting( CVarUtils::CreateCVar<>( "debug.Lighting", true, "Turn lighting debugging on") ) 
    {}

    bool&  m_bDebugLighting;
};

#endif
