#include <SceneGraph/GLObject.h>

namespace SceneGraph
{

std::map<int,GLObject*> GLObject::g_mObjects;
int GLObject::g_nHandleCounter = 1;

/////////////////////////////////////////////////////////////////////////////////
GLObject::GLObject()
    : m_sObjectName("unnamed-object"), m_bVisible(true), m_bPerceptable(true),
      m_bIgnoreDepth(false),
      m_T_po(Eigen::Matrix4d::Identity()), m_dScale(1,1,1), m_bIsSelectable(false),
      m_nDisplayList(-1)
{
}

/////////////////////////////////////////////////////////////////////////////////
GLObject::GLObject( const std::string& name)
    : m_sObjectName(name), m_bVisible(true), m_bPerceptable(true),
      m_bIgnoreDepth(false),
      m_T_po(Eigen::Matrix4d::Identity()), m_dScale(1,1,1), m_bIsSelectable(false),
      m_nDisplayList(-1)
{
}

/////////////////////////////////////////////////////////////////////////////////
GLObject::GLObject( const GLObject& rhs )
{
    *this = rhs;
}

/////////////////////////////////////////////////////////////////////////////////
GLObject::~GLObject()
{
#ifndef HAVE_GLES
    if(m_nDisplayList >= 0) {
        glDeleteLists(m_nDisplayList,1);
    }
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////////
int GLObject::AllocSelectionId()
{
    int handle = g_nHandleCounter++;
    g_mObjects[ handle ] = this;
    return handle;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void GLObject::DrawChildren(RenderMode renderMode)
{
    for(std::vector<GLObject*>::const_iterator i=m_vpChildren.begin(); i!= m_vpChildren.end(); ++i)
    {
        (*i)->DrawObjectAndChildren(renderMode);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void GLObject::DrawObjectAndChildren(RenderMode renderMode)
{
    if( IsVisible() && (
            (renderMode == eRenderVisible) || (renderMode == eRenderNoPrePostHooks) ||
            (renderMode == eRenderSelectable && (IsSelectable() || m_vpChildren.size() > 0) ) ||
			(renderMode == eRenderPerceptable && IsPerceptable())
		)
	) {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        
        if(m_bIgnoreDepth) {
            glDepthMask(false);
            glDisable(GL_DEPTH_TEST);            
        }

#ifdef HAVE_GLES
        Eigen::Matrix4f T_po = m_T_po.cast<float>();
        glMultMatrixf(T_po.data());
        glScalef(m_dScale[0],m_dScale[1],m_dScale[2]);
        DrawCanonicalObject();
#else
        glMultMatrixd(m_T_po.data());
        glScaled(m_dScale[0],m_dScale[1],m_dScale[2]);

        if(m_nDisplayList >= 0) {
            glCallList( m_nDisplayList );
        }else{
            DrawCanonicalObject();
        }
#endif //HAVE_GLES

        if(m_bIgnoreDepth) {
            glDepthMask(true);
            glEnable(GL_DEPTH_TEST);
        }

        DrawChildren();

        glPopMatrix();
    }
}

void GLObject::CompileAsGlCallList()
{
#ifndef HAVE_GLES
    if( m_nDisplayList == -1 ){
        m_nDisplayList = glGenLists(1);
        glNewList( m_nDisplayList, GL_COMPILE );
        DrawCanonicalObject();
        glEndList();
    }
#endif // HAVE_GLES
}

/////////////////////////////////////////////////////////////////////////////////
void GLObject::SetVisible(bool visible)
{
    m_bVisible = visible;
}

/////////////////////////////////////////////////////////////////////////////////
bool GLObject::IsVisible() const
{
    return m_bVisible;
}

/////////////////////////////////////////////////////////////////////////////////
bool GLObject::IsPerceptable() const
{
    return m_bPerceptable;
}

/////////////////////////////////////////////////////////////////////////////////
void GLObject::SetPerceptable( bool bPerceptable )
{
    m_bPerceptable = bPerceptable;
}

/////////////////////////////////////////////////////////////////////////////////
void GLObject::SetIgnoreDepth( bool bIgnoreDepth )
{
    m_bIgnoreDepth = bIgnoreDepth;
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
void GLObject::AddChild( GLObject* pChild )
{
    m_vpChildren.push_back( pChild );
}

bool GLObject::RemoveChild( GLObject* pChild )
{
    for(size_t ii = 0 ; ii < m_vpChildren.size() ; ii++) {
        if(m_vpChildren[ii] == pChild ){
            m_vpChildren.erase(m_vpChildren.begin()+ii);
            return true;
        }
    }

    return false;
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
Eigen::Matrix4d GLObject::GetPose4x4_po() const
{
    return m_T_po;
}

///////////////////////////////////////////////////////////////////////////////////////////
Eigen::Matrix4d GLObject::GetPose4x4_op() const
{
    Eigen::Matrix4d T_op;
    T_op.block<3,3>(0,0) = m_T_po.block<3,3>(0,0).transpose();
    T_op.block<3,1>(0,3) = - T_op.block<3,3>(0,0) * m_T_po.block<3,1>(0,3);
    T_op.block<1,4>(3,0) << 0.0, 0.0, 0.0, 1.0;
    return T_op;
}

///////////////////////////////////////////////////////////////////////////////////////////
Eigen::Vector6d GLObject::GetPose() const
{
    return GLT2Cart(m_T_po);
}

///////////////////////////////////////////////////////////////////////////////////////////
void GLObject::SetPosition(Eigen::Vector3d v)
{
    SetPosition(v(0),v(1),v(2));
}

///////////////////////////////////////////////////////////////////////////////////////////
void GLObject::SetPosition(double x, double y, double z)
{
    SetPose(x,y,z,0,0,0);
}

////////////////////////////////////////////////////////////////////////////////////////////
void GLObject::SetPose(const Eigen::Vector6d& v)
{
    SetPose(v(0), v(1), v(2), v(3), v(4), v(5));
}

void GLObject::SetPose(const Eigen::Matrix4d& T_po)
{
	m_T_po = T_po;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
void GLObject::SetPose(double x, double y, double z, double r, double p, double q)
{
    m_T_po = GLCart2T(x,y,z,r,p,q);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void GLObject::SetScale(double s) {
    m_dScale = Eigen::Vector3d(s,s,s);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void GLObject::SetScale(const Eigen::Vector3d& s) {
    m_dScale = s;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
Eigen::Vector3d GLObject::GetScale() {
    return m_dScale;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
bool GLObject::IsSelectable()
{
    return m_bIsSelectable;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
const AxisAlignedBoundingBox& GLObject::ObjectBounds() const
{
    return m_aabb;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
AxisAlignedBoundingBox GLObject::ChildrenBounds() const
{
    AxisAlignedBoundingBox bbox;
    for(std::vector<GLObject*>::const_iterator i=m_vpChildren.begin(); i!= m_vpChildren.end(); ++i) {
        if((*i)->IsVisible()) {
            bbox.Insert((*i)->m_T_po, (*i)->ObjectAndChildrenBounds() );
        }
    }
    return bbox;    
}

/////////////////////////////////////////////////////////////////////////////////////////////////
AxisAlignedBoundingBox GLObject::ObjectAndChildrenBounds() const
{
    AxisAlignedBoundingBox bbox = ObjectBounds();
    bbox.Insert(ChildrenBounds());
    return bbox;
}




} // SceneGraph
