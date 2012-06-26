#ifndef _GL_TEXT_H_
#define _GL_TEXT_H_

#include <SceneGraph/GLObject.h>
#include <SceneGraph/GLColor.h>

#include <string>


class GLText : public GLObject
{
    public:

		GLText()
        {
			InitReset();
            m_sObjectName = "Text";
            m_bPerceptable = false;
        }

		void InitReset()
		{
           	m_nX = 0;
        	m_nY = 0;
        	m_Color = GLColor();
			m_sText = "";
			m_nFontID = 0;
			m_nFontSize = 10;
		}

        void draw()
        {
        	PushOrtho( WindowWidth(), WindowHeight() );
            glColor4f( m_Color.r, m_Color.g, m_Color.b, m_Color.a );
            gl_font( m_nFontID, m_nFontSize );
            glRasterPos2f( m_nX, m_nY );
            gl_draw( m_sText.c_str(), m_sText.length() );
            PopOrtho();
        }

        void SetText( const std::string& sText )
		{
			m_sText = sText;
        }

        void SetSize( int nFontSize )
		{
        	assert(nFontSize > 0);
			m_nFontSize = nFontSize;
        }

        void SetPos( unsigned int X, unsigned int Y )
		{
        	m_nX = X;
        	m_nY = Y;
        }

        void ClearText()
		{
        	m_sText.clear();
        }

    private:
        std::string 	m_sText;
        unsigned int	m_nX;
        unsigned int	m_nY;
        int          	m_nFontID;
        int          	m_nFontSize;
        GLColor         	m_Color;
};


#endif
