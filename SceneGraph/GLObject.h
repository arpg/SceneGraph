#ifndef _GL_OBJECT_
#define _GL_OBJECT_

#undef Success // why ?
#include <SceneGraph/GLHelpers.h>
#include <SceneGraph/AxisAlignedBoundingBox.h>
#include <Eigen/Eigen>
#include <string>
#include <vector>
#include <map>
#include <stdio.h>

#ifdef _ANDROID_
#include <SceneGraph/GLES_compat.h>
#endif // _ANDROID_


#ifdef __GNUC__
#  define SCENEGRAPH_DEPRECATED __attribute__((deprecated))
#elif defined _MSC_VER
#  define SCENEGRAPH_DEPRECATED __declspec(deprecated)
#else
#  define SCENEGRAPH_DEPRECATED
#endif

namespace Eigen{
    typedef Matrix<double, 6, 1> Vector6d;
    typedef std::vector<Eigen::Vector6d,Eigen::aligned_allocator<Eigen::Vector6d> > Vector6dAlignedVec;
    typedef std::vector<Eigen::Vector3d,Eigen::aligned_allocator<Eigen::Vector3d> > Vector3dAlignedVec;
}

namespace SceneGraph
{

enum MouseButton
{
  MouseButtonLeft = 1,
  MouseButtonMiddle = 2,
  MouseButtonRight = 4,
  MouseWheelUp = 8,
  MouseWheelDown = 16
};

enum KeyModifier
{
  KeyModifierShift = 1<<16,
  KeyModifierCtrl  = 1<<17,
  KeyModifierAlt   = 1<<18
};

enum RenderMode
{
#ifdef HAVE_GLES
    eRenderVisible = 1,
    eRenderSelectable = 0,
#else
	eRenderVisible = GL_RENDER,    
	eRenderSelectable = GL_SELECT,
#endif
    eRenderPerceptable,
    eRenderNoPrePostHooks
};

class GLObject
{
    public:

        /////////////////////////////////
        // GLObject Constructors
        /////////////////////////////////

        GLObject();
        GLObject( const std::string& name);
        GLObject( const GLObject& rhs );

        virtual ~GLObject();

        /////////////////////////////////
        // Drawing methods
        /////////////////////////////////

        /// Draw Canonical object (i.e. without pose transform applied)
        /// Overide this in your derived classes.
        virtual void DrawCanonicalObject() = 0;

        /// Draw Children (without pose transform applied)
        void DrawChildren(RenderMode renderMode = eRenderVisible);

        /// Apply object transform, then draw object and its children (recursively)
        /// Although virtual, you should NOT overide this method unless you have
        /// a very good reason. Implement DrawCanonicalObject instead.
        virtual void DrawObjectAndChildren(RenderMode renderMode = eRenderVisible);

        /// Compile object as GL Calllist which will be called automatically
        /// instead of DrawCanonical
        virtual void CompileAsGlCallList();

        /////////////////////////////////
        // Interaction methods: can be overridden
        /////////////////////////////////

        virtual bool Mouse(int button, const Eigen::Vector3d& win, const Eigen::Vector3d& obj, const Eigen::Vector3d& normal, bool pressed, int button_state, int pickId);

        virtual bool MouseMotion(const Eigen::Vector3d& win, const Eigen::Vector3d& obj, const Eigen::Vector3d& normal, int button_state, int pickId);

        /////////////////////////////////
        // Object Properties
        /////////////////////////////////

        const std::string& ObjectName() const;
        void SetObjectName( const std::string& sName );

        /// Get / Set wether this object should be drawn
        bool IsVisible() const;
        void SetVisible(bool visible = true);

        /// can be measured (e.g., not a virtual thing)
        bool IsPerceptable() const;
        void SetPerceptable( bool bPerceptable );

        /// Ignore depth buffer when rendering this object
        void SetIgnoreDepth( bool bIgnoreDepth );

        /////////////////////////////////
        // Object Pose
        /////////////////////////////////

        /// Return Object (o) to Parent (p) 4x4 transformation matrix
        /// (as used by OpenGL) such that x_p = GetPose4x4_po() * x_o;
        Eigen::Matrix4d GetPose4x4_po() const;

        /// Return pose as (x,y,z,roll,pitch,yaw) vector
        Eigen::Vector6d GetPose() const;

        /// Set position only (x,y,z)
        void SetPosition(Eigen::Vector3d v);

        /// Set position only (x,y,z)
        void SetPosition(double x, double y, double z = 0);

        /// Set pose as (x,y,z,roll,pitch,yaw) vector
        void SetPose(const Eigen::Vector6d& v);

		/// Set pose as 4x4 matrix
        void SetPose(const Eigen::Matrix4d& T_po);

        /// Set pose using x,y,z,roll,pitch,yaw parametrisation
        void SetPose(double x, double y, double z, double p, double q, double r);

        /// Set objects scale
        void SetScale(double s);
        void SetScale(const Eigen::Vector3d& s);
        Eigen::Vector3d GetScale();

        /////////////////////////////////
        // Object Size
        /////////////////////////////////
        
        /// Return this Objects axis aligned extent (in object coordinate frame)
        const AxisAlignedBoundingBox& ObjectBounds() const;
        
        /// Return this Objects children axis aligned extent (in object coordinate frame)
        AxisAlignedBoundingBox ChildrenBounds() const;                

        /// Return the extent of this object including all visible children
        /// (in object coordinate frame)
        AxisAlignedBoundingBox ObjectAndChildrenBounds() const;

        /////////////////////////////////
        // Children
        /////////////////////////////////

        void AddChild( GLObject* pChild );
        bool RemoveChild( GLObject* pChild );

        size_t NumChildren() const;

        /// Access child objects
        GLObject& operator[](int i);
        const GLObject& operator[](int i) const;

    protected:
        int AllocSelectionId();
        bool IsSelectable();

        //! Vector of children whose poses are expressed relative to
        //! this object
        std::vector<GLObject*>         m_vpChildren;

        //! Object name
        std::string                    m_sObjectName;

        //! Whether this object should be drawn
        bool                           m_bVisible;
        
        //! can be measured (e.g., not a virtual thing)
        bool                           m_bPerceptable;

        //! Whether we should ignore depth tests for this object
        bool                           m_bIgnoreDepth;
        
        //! Object to Parent transform. Includes position, rotation (x_p = m_T_po & m_dScale * x_o)
        Eigen::Matrix4d                m_T_po;
        Eigen::Vector3d                m_dScale;

        // Extent of this GLObject
        AxisAlignedBoundingBox         m_aabb;

        //! Can be selected
        bool                           m_bIsSelectable;

        // static map of id to objects
        static std::map<int,GLObject*> g_mObjects;
        static int g_nHandleCounter;

        GLint                          m_nDisplayList;
};

inline bool GLObject::Mouse(int, const Eigen::Vector3d&, const Eigen::Vector3d&, const Eigen::Vector3d&, bool, int, int) {
    return false;
}

inline bool GLObject::MouseMotion(const Eigen::Vector3d&, const Eigen::Vector3d&, const Eigen::Vector3d&, int, int) {
    return false;
}



} // SceneGraph

#endif
