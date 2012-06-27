#ifndef _GL_OBJECT_
#define _GL_OBJECT_

#undef Success
#include <Eigen/Eigen>

#include <SceneGraph/GLHelpers.h>

#include <string>
#include <vector>
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

        /////////////////////////////////
        // Interaction methods: can be overridden
        /////////////////////////////////

        virtual void select( unsigned int ) {}
        virtual void drag() {}
        virtual void release() {}

        /////////////////////////////////
        // Object Properties
        /////////////////////////////////

        const std::string& ObjectName() const;
        void SetObjectName( const std::string& sName );

        /// Get / Set Object Id for picking perposes
        unsigned int Id();
        void SetId( unsigned int nId );

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

        Eigen::Vector6d& GetPoseRef();

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


        Eigen::Vector3d GetPosUnderCursor();

		Eigen::Vector2i GetCursorPos(); 

        /// such as an image
        bool Is2dLayer()
        {
            return m_bIs2dLayer;
        }

    protected:
        std::vector<GLObject*>    m_vpChildren;

        std::string               m_sObjectName;
        bool                      m_bPerceptable; //< can be measured (e.g., not a virtual thing)
        unsigned int              m_nId;      //< Object handle
        bool                      m_bVisible;
        bool                      m_bIs2dLayer; //< such as an image
        Eigen::Vector6d           m_dPosition; //< Object position

        // Child to Parent transform. Includes position, rotation and scale (x_p = m_T_pc & x_c)
        Eigen::Matrix4d           m_T_pc;
};

} // SceneGraph

#endif
