#ifndef _GL_OBJECT_
#define _GL_OBJECT_

#undef Success
#include <Eigen/Eigen>

#include <SceneGraph/GLHelpers.h>

#include <string>
#include <vector>
#include <map>
#include <stdio.h>

namespace Eigen{
    typedef Matrix<double, 6, 1> Vector6d;
}

namespace SceneGraph
{

class GLObject
{
    public:

        /////////////////////////////////
        // GLObject Constructors
        /////////////////////////////////

        GLObject();
        GLObject( const std::string& name);
        GLObject( const GLObject& rhs );

        /////////////////////////////////
        // Drawing methods
        /////////////////////////////////

        /// Draw Canonical object (i.e. without pose transform applied)
        /// Overide this in your derived classes.
        virtual void DrawCanonicalObject() = 0;

        /// Apply object transform, then draw object and its children (recursively)
        void DrawObjectAndChildren();

        /// Alow this object to draw itself as a functor
        inline void operator()() {
            DrawObjectAndChildren();
        }

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
        void SetPose(Eigen::Vector6d v);

        /// Set pose using x,y,z,roll,pitch,yaw parametrisation
        void SetPose(double x, double y, double z, double p, double q, double r);

        /// Note: This method is a little dangerous right now.
        /// Once we encapsulate pose as a similarity transform we will
        /// be better offer.
        void Scale(double s);

        /////////////////////////////////
        // Children
        /////////////////////////////////

        void AddChild( GLObject* pChild );

        size_t NumChildren() const;

        /// Access child objects
        GLObject& operator[](int i);
        const GLObject& operator[](int i) const;

    protected:
        int AllocSelectionId();

        //! Vector of children whose poses are expressed relative to
        //! this object
        std::vector<GLObject*>    m_vpChildren;

        //! Object name
        std::string               m_sObjectName;

        //! Whether this object should be drawn
        bool                      m_bVisible;

        //! can be measured (e.g., not a virtual thing)
        bool                      m_bPerceptable;

        //! Object to Parent transform. Includes position, rotation and scale (x_p = m_T_po & x_o)
        Eigen::Matrix4d           m_T_po;

        // static map of id to objects
        static std::map<int,GLObject*> g_mObjects;
        static int g_nHandleCounter;

};

inline bool GLObject::Mouse(int, const Eigen::Vector3d&, const Eigen::Vector3d&, const Eigen::Vector3d&, bool, int, int) {
    return false;
}

inline bool GLObject::MouseMotion(const Eigen::Vector3d&, const Eigen::Vector3d&, const Eigen::Vector3d&, int, int) {
    return false;
}



} // SceneGraph

#endif
