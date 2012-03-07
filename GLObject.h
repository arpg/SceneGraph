#ifndef _GL_OBJECT_
#define _GL_OBJECT_

class GLWindow;

#include <string>

class GLObject
{
    public:

        GLObject()
        {
            m_bVisible = true;
        }

        // User supplied gl drawing routine
		virtual void draw() = 0;

        virtual void select( unsigned int ) {};

        virtual void drag() {}

		virtual void release() {}

        // set the parent window pointer
        void init( GLWindow* pWin )
        {
            m_pWin = pWin;
        }

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

        void SetVisible()
        {
            m_bVisible = true;
        }

        void SetInVisible()
        {
            m_bVisible = false;
        }

        bool IsVisible()
        {
            return m_bVisible;
        }

        void SetName( const std::string& sName )
        {
            m_sObjectName = sName;
        }

        const char* ObjectName()
        {
            return m_sObjectName.c_str();
        }

        void SetId( unsigned int nId )
        {
            m_nId = nId;
        }

        unsigned int Id()
        {
            return m_nId;
        }

    protected:
        std::string     m_sObjectName;

    private:
        unsigned int    m_nId;          // Object handle
        GLWindow*  		m_pWin; 		//< The window the object belongs to
        bool            m_bVisible;
};



#endif
