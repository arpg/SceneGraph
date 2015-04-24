#include <SceneGraph/GLObject.h>
#include <SceneGraph/GLSceneGraph.h>
#include <SceneGraph/GLLight.h>

namespace SceneGraph
{

extern std::map<int,GLObject*>   g_mObjects; // map of id to objects

/////////////////////////////////////////////////////////////////////////////////
GLSceneGraph::GLSceneGraph()
    : GLObject("SceneGraph"), m_bEnableLighting(false)
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

    m_vpPrePostRender.clear();

    // This object and children can be selected
    m_bIsSelectable = true;
}

/////////////////////////////////////////////////////////////////////////////////
void GLSceneGraph::Reset()
{
    Clear();
}

/////////////////////////////////////////////////////////////////////////////////
void GLSceneGraph::AddChild( GLObject* pChild )
{
    GLObjectPrePostRender* prepost = dynamic_cast<GLObjectPrePostRender*>(pChild);
    GLLight* light = dynamic_cast<GLLight*>(pChild);

    if(light) {
        m_bEnableLighting = true;
    }

    if(prepost) {
        m_vpPrePostRender.push_back(prepost);
    }

    GLObject::AddChild(pChild);
}

/////////////////////////////////////////////////////////////////////////////////
bool GLSceneGraph::RemoveChild( GLObject* pChild )
{
    GLObjectPrePostRender* prepost = dynamic_cast<GLObjectPrePostRender*>(pChild);
    if(prepost) {
        for(size_t ii = 0 ; ii < m_vpPrePostRender.size() ;ii++ ) {
            if(m_vpPrePostRender[ii] == prepost){
                m_vpPrePostRender.erase(m_vpPrePostRender.begin()+ii);
                break;
            }
        }
    }

    return GLObject::RemoveChild(pChild);
}



/////////////////////////////////////////////////////////////////////////////////
void GLSceneGraph::DrawCanonicalObject()
{
    // Nothing to do here
}

void GLSceneGraph::DrawObjectAndChildren(RenderMode renderMode)
{
    pangolin::GlState gl;
    
    glPushMatrix();
    glMultMatrixd(m_T_po.data());
    glScaled(m_dScale[0],m_dScale[1],m_dScale[2]);

    if(m_bEnableLighting) {
        gl.glEnable( GL_LIGHTING );
        gl.glEnable( GL_COLOR_MATERIAL );
#ifndef HAVE_GLES
        glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
#endif
        glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    }else{
        gl.glDisable( GL_LIGHTING );
        gl.glDisable( GL_COLOR_MATERIAL );
        glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    }

    if(renderMode == eRenderNoPrePostHooks || renderMode == eRenderSelectable) {
        DrawChildren(renderMode);
    }else{
        for(unsigned int l=0; l < m_vpPrePostRender.size(); ++l) {
            m_vpPrePostRender[l]->PreRender(*this);
        }
        DrawChildren(renderMode);
        for(unsigned int l=0; l < m_vpPrePostRender.size(); ++l) {
            m_vpPrePostRender[l]->PostRender(*this);
        }
    }

    glPopMatrix();
}

void GLSceneGraph::ApplyPreferredGlSettings()
{
    // Default to transparent white background for better screenshots
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    
    // Disable multisample for general use (it messes up antialiased lines)
    // Enable individually for particular GLObjects
    glDisable(GL_MULTISAMPLE);
    
//    // GL_POLYGON_SMOOTH is known to be really bad.
//    glEnable(GL_POLYGON_SMOOTH);
//    glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
    
    glEnable(GL_NORMALIZE);

    // Antialiased lines work great, but should be disabled if multisampling is enabled
    glEnable(GL_LINE_SMOOTH);
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );

    // Enable alpha blending
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Enable back face culling by default. Rendering quality and speed is improved
//    glEnable(GL_CULL_FACE);
    
    // Shading model to use when lighing is enabled
    glShadeModel(GL_SMOOTH);
    
    glDepthFunc( GL_LEQUAL );
    glEnable( GL_DEPTH_TEST );

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

/////////////////////////////////////////////////////////////////////////////////



} // SceneGraph
