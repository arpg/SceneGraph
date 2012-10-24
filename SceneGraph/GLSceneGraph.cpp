#include <SceneGraph/GLObject.h>
#include <SceneGraph/GLSceneGraph.h>

namespace SceneGraph
{

extern std::map<int,GLObject*>   g_mObjects; // map of id to objects

/////////////////////////////////////////////////////////////////////////////////
GLSceneGraph::GLSceneGraph()
    : GLObject("SceneGraph"), m_bShowLights(false), m_bShowShaddows(false)
{
    Reset();
}

/////////////////////////////////////////////////////////////////////////////////
GLObject* GLSceneGraph::Root()
{
    return this;
}

/////////////////////////////////////////////////////////////////////////////////
void GLSceneGraph::Clear()
{
    // Remove children
    m_vpChildren.clear();

    // Clear lights
    for(unsigned int l=0; l < m_vpLights.size(); ++l) {
        glDisable( GL_LIGHT0 + l );
        delete m_vpLights[l];
    }
    m_vpLights.clear();

    // This object and children can be selected
    m_bIsSelectable = true;
}

/////////////////////////////////////////////////////////////////////////////////
void GLSceneGraph::Reset()
{
    Clear();
}

/////////////////////////////////////////////////////////////////////////////////
GLLight& GLSceneGraph::AddLight(Eigen::Vector3d pos)
{
    GLLight* light = new GLLight();
    light->SetPosition(pos);
    light->SetVisible(m_bShowLights);

    m_vpLights.push_back(light);
    AddChild(light);

    return *light;
}

/////////////////////////////////////////////////////////////////////////////////
GLLight& GLSceneGraph::GetLight(unsigned int i)
{
    assert(i < m_vpLights.size());
    return *m_vpLights[i];
}

/////////////////////////////////////////////////////////////////////////////////
void GLSceneGraph::ShowLights(bool showLights)
{
    m_bShowLights = showLights;
    for(unsigned int l=0; l < m_vpLights.size(); ++l) {
        m_vpLights[l]->SetVisible(showLights);
    }
}

void GLSceneGraph::ShowShaddows(bool showShaddows)
{
    m_bShowShaddows = showShaddows;
}

/////////////////////////////////////////////////////////////////////////////////
void GLSceneGraph::DrawCanonicalObject()
{
    for(unsigned int l=0; l < m_vpLights.size(); ++l) {
        m_vpLights[l]->ApplyAsGlLight(GL_LIGHT0 + l);
    }
}

void GLSceneGraph::DrawObjectAndChildren(RenderMode renderMode )
{
    glPushAttrib(GL_ENABLE_BIT);

    if(m_vpLights.size() > 0) {
        glEnable( GL_LIGHTING );
        glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
        glEnable( GL_COLOR_MATERIAL );
        glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    }else{
        glDisable( GL_LIGHTING );
        glDisable( GL_COLOR_MATERIAL );
        glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
    }

    GLObject::DrawObjectAndChildren(renderMode);

    glPopAttrib();
}

void GLSceneGraph::ApplyPreferredGlSettings()
{
    glShadeModel(GL_SMOOTH);

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_NORMALIZE);

    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
    glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );

    glDepthFunc( GL_LEQUAL );
    glEnable( GL_DEPTH_TEST );

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    glLineWidth(1.5);
}


/////////////////////////////////////////////////////////////////////////////////
GLObject* GLSceneGraph::GetObject( unsigned int nId )
{
    std::map<int,GLObject*>::iterator it = g_mObjects.find( nId );
    if( it == g_mObjects.end() ){
        return NULL;
    }
    return it->second;
}

} // SceneGraph
