#ifndef _GL_OBJECT_
#define _GL_OBJECT_

#undef Success
#include <Eigen/Eigen>

class GLWindow;

#include <string>
#include <vector>
#include <stdio.h>

namespace Eigen{
    typedef Matrix<double, 6, 1> Vector6d;
}
class GLObject
{
    public:

        GLObject();

        GLObject( const GLObject& rhs )
        {
            *this = rhs;
        }

        // User supplied gl drawing routine
		virtual void draw() = 0;

        virtual void select( unsigned int ) {};

        virtual void MouseOver( unsigned int ) {};

        virtual void drag() {}

		virtual void release() {}
    
        // Get top,left,bottom,right coordinates of the object
        // in the window (if it's a 2d object)
        virtual void GetBoundingBox(int&, int&, int&, int&) {}

        GLWindow* Window();

        void SetVisible();

        void SetInVisible();

        bool IsVisible();

        void SetName( const std::string& sName );
        
        const char* ObjectName();

        void SetId( unsigned int nId );

        unsigned int Id();

        /// set the parent window pointer
        void InitWindowPtr( GLWindow* pWin );

		// Check if window is valid
		bool valid(); 

        /// Get parent window width.
		int WindowWidth(); 

        /// Get parent window height.
        int WindowHeight(); 

        /// If the name nId is selected
        bool IsSelected( unsigned int nId );

        /// Unselect name nId
        void UnSelect( unsigned int nId );

        unsigned int AllocSelectionId(); 

        Eigen::Vector3d GetPosUnderCursor();

		Eigen::Vector2i GetCursorPos(); 

        void AddChild( GLObject* pChild ); 

        std::vector< GLObject* >  m_vpChildren;

        /// such as an image
        bool Is2dLayer()
        {
            return m_bIs2dLayer;
        }

        /// can be measured (e.g., not a virtual thing)
        bool IsPerceptable()
        {
            return m_bPerceptable;
        } 

        void SetPerceptable( bool bPerceptable )
        {
            m_bPerceptable = bPerceptable;
        }

        Eigen::Vector6d GetPose();

        Eigen::Vector6d& GetPoseRef();

        void SetPose(Eigen::Vector6d v);

        void SetPose(double x, double y, double z, double p, double q, double r);

//    private:
        void  _RecursivelyInitObjects( GLObject* pObj, GLWindow* pWin );

    protected:
        std::string               m_sObjectName;
        GLObject*                 m_pParent;
        bool                      m_bPerceptable; //< can be measured (e.g., not a virtual thing)
        unsigned int              m_nId;      //< Object handle
        GLWindow*                 m_pWin;     //< The window the object belongs to
        bool                      m_bVisible;
        bool                      m_bIs2dLayer; //< such as an image
        Eigen::Vector6d           m_dPosition; //< Object position
};


#endif
