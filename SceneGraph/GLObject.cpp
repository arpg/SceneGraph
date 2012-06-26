#include "GLObject.h"
#include <map>

std::map<int,GLObject*>   g_mObjects; // map of id to objects
int                       g_nHandleCounter;

/////////////////////////////////////////////////////////////////////////////////
GLObject::GLObject()
{
    m_bVisible = true;
    m_bIs2dLayer = false; //< such as an image 
    m_bPerceptable = true; //< can be measured (e.g., not a virtual thing)
    m_sObjectName = "unnamed-object";
    m_dPosition << 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f;
}

/////////////////////////////////////////////////////////////////////////////////
void GLObject::SetVisible()
{
    m_bVisible = true;
}

/////////////////////////////////////////////////////////////////////////////////
void GLObject::SetInVisible()
{
    m_bVisible = false;
}

/////////////////////////////////////////////////////////////////////////////////
bool GLObject::IsVisible()
{
    return m_bVisible;
}

/////////////////////////////////////////////////////////////////////////////////
void GLObject::SetName( const std::string& sName )
{
    m_sObjectName = sName;
}

/////////////////////////////////////////////////////////////////////////////////
const char* GLObject::ObjectName()
{
    return m_sObjectName.c_str();
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
bool GLObject::IsSelected( unsigned int nId )
{
    // TODO: Implement this functionality
//    return m_pWin->IsSelected( nId );
}

/////////////////////////////////////////////////////////////////////////////////
void GLObject::UnSelect( unsigned int nId )
{
    // TODO: Implement this functionality
//    m_pWin->UnSelect( nId );
}

/////////////////////////////////////////////////////////////////////////////////
unsigned int GLObject::AllocSelectionId()
{
    // TODO: Implement this functionality
//    unsigned int nId =  m_pWin->AllocSelectionId( this );
//    return nId;
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
bool GLObject::valid()
{
    // TODO: Implement this functionality?
//    if( m_pWin && m_pWin->valid() ) {
//        return true;
//    }
//    return false;
}

/////////////////////////////////////////////////////////////////////////////////
void GLObject::AddChild( GLObject* pChild )
{
    m_vpChildren.push_back( pChild );
    pChild->m_pParent = this;
    g_mObjects[ g_nHandleCounter ] = pChild;
    pChild->SetId( g_nHandleCounter ); 
    //printf("adding %s in slot %d\n", pChild->m_sObjectName.c_str(), g_nHandleCounter );
    g_nHandleCounter++;

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
