#include "GLObject.h"

#include <map>

namespace SceneGraph
{

std::map<int,GLObject*>   g_mObjects; // map of id to objects
int                       g_nHandleCounter;

/////////////////////////////////////////////////////////////////////////////////
GLObject::GLObject()
    : m_sObjectName("unnamed-object"), m_bVisible(true), m_bIs2dLayer(false),
      m_bPerceptable(true), m_T_pc(Eigen::Matrix4d::Identity())
{
    m_dPosition << 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f;
}

/////////////////////////////////////////////////////////////////////////////////
GLObject::GLObject( const std::string& name)
    : m_sObjectName(name), m_bVisible(true), m_bIs2dLayer(false),
      m_bPerceptable(true), m_T_pc(Eigen::Matrix4d::Identity())
{
    m_dPosition << 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f;
}

/////////////////////////////////////////////////////////////////////////////////
GLObject::GLObject( const GLObject& rhs )
{
    *this = rhs;
}

void GLObject::DrawObjectAndChildren()
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixd(m_T_pc.data());

    DrawCanonicalObject();

    for(std::vector<GLObject*>::const_iterator i=m_vpChildren.begin(); i!= m_vpChildren.end(); ++i)
    {
        (*i)->DrawObjectAndChildren();
    }

    glPopMatrix();
}

/////////////////////////////////////////////////////////////////////////////////
void GLObject::SetVisible(bool visible)
{
    m_bVisible = visible;
}

/////////////////////////////////////////////////////////////////////////////////
bool GLObject::IsVisible()
{
    return m_bVisible;
}

/////////////////////////////////////////////////////////////////////////////////
bool GLObject::IsPerceptable()
{
    return m_bPerceptable;
}

/////////////////////////////////////////////////////////////////////////////////
void GLObject::SetPerceptable( bool bPerceptable )
{
    m_bPerceptable = bPerceptable;
}

/////////////////////////////////////////////////////////////////////////////////
void GLObject::SetObjectName( const std::string& sName )
{
    m_sObjectName = sName;
}

/////////////////////////////////////////////////////////////////////////////////
const std::string& GLObject::ObjectName() const
{
    return m_sObjectName;
}

/////////////////////////////////////////////////////////////////////////////////
void GLObject::SetId( unsigned int nId )
{
    m_nId = nId;
}

/////////////////////////////////////////////////////////////////////////////////
unsigned int GLObject::Id()
{
    return m_nId;
}

/////////////////////////////////////////////////////////////////////////////////
Eigen::Vector3d GLObject::GetPosUnderCursor()
{
    // TODO: Implement this functionality?
//    return m_pWin->GetPosUnderCursor();
}

/////////////////////////////////////////////////////////////////////////////////
Eigen::Vector2i GLObject::GetCursorPos()
{
    // TODO: Implement this functionality?
//    return m_pWin->GetCursorPos();
}

/////////////////////////////////////////////////////////////////////////////////
void GLObject::AddChild( GLObject* pChild )
{
    m_vpChildren.push_back( pChild );
    g_mObjects[ g_nHandleCounter ] = pChild;
    pChild->SetId( g_nHandleCounter ); 
    //printf("adding %s in slot %d\n", pChild->m_sObjectName.c_str(), g_nHandleCounter );
    g_nHandleCounter++;
}

/////////////////////////////////////////////////////////////////////////////////
size_t GLObject::NumChildren() const
{
    return m_vpChildren.size();
}

/////////////////////////////////////////////////////////////////////////////////
GLObject& GLObject::operator[](int i)
{
    return *m_vpChildren[i];
}

/////////////////////////////////////////////////////////////////////////////////
const GLObject& GLObject::operator[](int i) const
{
    return *m_vpChildren[i];
}

///////////////////////////////////////////////////////////////////////////////////////////
Eigen::Vector6d GLObject::GetPose()
{
    return m_dPosition;
}


///////////////////////////////////////////////////////////////////////////////////////////
Eigen::Vector6d& GLObject::GetPoseRef()
{
    return m_dPosition;
}


////////////////////////////////////////////////////////////////////////////////////////////
void GLObject::SetPose(Eigen::Vector6d v)
{
    m_dPosition = v;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
void GLObject::SetPose(double x, double y, double z, double p, double q, double r)
{
    m_dPosition[0] = x;
    m_dPosition[1] = y;
    m_dPosition[2] = z;
    m_dPosition[3] = p;
    m_dPosition[4] = q;
    m_dPosition[5] = r;
}

} // SceneGraph
