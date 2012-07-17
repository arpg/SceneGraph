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

        /// Apply object transform, then draw object and its children (recursively)
        void DrawObjectAndChildren();

        /// Draw Canonical object (i.e. without pose transform applied)
        virtual void DrawCanonicalObject() = 0;

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
        bool IsVisible();
        void SetVisible(bool visible = true);

        /// can be measured (e.g., not a virtual thing)
        bool IsPerceptable();
        void SetPerceptable( bool bPerceptable );


        /////////////////////////////////
        // Object Pose
        /////////////////////////////////

        Eigen::Vector6d GetPose();

        void SetPose(Eigen::Vector6d v);

        void SetPose(double x, double y, double z, double p, double q, double r);


        /////////////////////////////////
        // Children
        /////////////////////////////////

        void AddChild( GLObject* pChild );

        size_t NumChildren() const;

        /// Access child objects
        GLObject& operator[](int i);
        const GLObject& operator[](int i) const;


        /////////////////////////////////
        // 2D Specific features - rethink these?
        /////////////////////////////////
    
        // Get top,left,bottom,right coordinates of the object
        // in the window (if it's a 2d object)
        virtual void GetBoundingBox(int&, int&, int&, int&) {}

        /// Get parent window width.
        int WindowWidth() { return 1; }

        /// Get parent window height.
        int WindowHeight() { return 1; }

        /// such as an image
        inline bool Is2dLayer()
        {
            return m_bIs2dLayer;
        }

    protected:
        void Init();
        int AllocSelectionId();

        std::vector<GLObject*>    m_vpChildren;

        std::string               m_sObjectName;
        bool                      m_bVisible;
        bool                      m_bPerceptable; //< can be measured (e.g., not a virtual thing)

        // Child to Parent transform. Includes position, rotation and scale (x_p = m_T_pc & x_c)
        Eigen::Matrix4d           m_T_pc;

        // Deprecate these?
        bool                      m_bIs2dLayer; //< such as an image

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
