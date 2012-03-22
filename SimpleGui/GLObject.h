#ifndef _GL_OBJECT_
#define _GL_OBJECT_

#include <Eigen/Eigen>

class GLWindow;

#include <string>
#include <vector>

class GLObject
{
    public:

        GLObject();

        // User supplied gl drawing routine
		virtual void draw() = 0;

        virtual void select( unsigned int ) {};

        virtual void drag() {}

		virtual void release() {}

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

    private:
        void  _RecursivelyInitObjects( GLObject* pObj, GLWindow* pWin );

    protected:
        std::string               m_sObjectName;
        GLObject*                 m_pParent;

    private:
        unsigned int              m_nId;      //< Object handle
        GLWindow*                 m_pWin;     //< The window the object belongs to
        bool                      m_bVisible;

};



#endif
