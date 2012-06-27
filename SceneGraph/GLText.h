#ifndef _GL_TEXT_H_
#define _GL_TEXT_H_

#include <SceneGraph/GLObject.h>
#include <SceneGraph/GLColor.h>

#include <GL/freeglut_std.h>
#include <GL/freeglut_ext.h>

#include <string>

namespace SceneGraph
{

static void* GLTextDefaultFont = GLUT_BITMAP_HELVETICA_12;

class GLText : public GLObject
{
    public:

        static void Draw(const std::string& text, void* font = GLTextDefaultFont )
        {
            glutBitmapString(font,(unsigned char*)text.c_str());
        }

        static void Draw(const std::string& text, float x, float y, void* font = GLTextDefaultFont)
        {
            glRasterPos2f(x,y);
            Draw(text, font);
        }

        static int Width(const std::string& text, void* font = GLTextDefaultFont )
        {
            return glutBitmapLength(font, (unsigned char*)text.c_str());
        }

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
//			m_nFontID = 0;
//			m_nFontSize = 10;
            m_font = GLTextDefaultFont;
		}

        void draw()
        {
        	PushOrtho( WindowWidth(), WindowHeight() );
            glColor4f( m_Color.r, m_Color.g, m_Color.b, m_Color.a );
//            gl_font( m_nFontID, m_nFontSize );
//            gl_draw( m_sText.c_str(), m_sText.length() );
            Draw(m_sText,m_nX, m_nY);
            PopOrtho();
        }

        void SetText( const std::string& sText )
		{
			m_sText = sText;
        }

        void SetSize( int nFontSize )
		{
            assert(nFontSize > 0);

            // TODO: Make this better somehow?
            if(nFontSize<=10) {
                m_font = GLUT_BITMAP_HELVETICA_10;
            }else if(nFontSize <= 12) {
                m_font = GLUT_BITMAP_HELVETICA_12;
            }else{
                m_font = GLUT_BITMAP_HELVETICA_18;
            }

//			m_nFontSize = nFontSize;
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
        void*           m_font;

        std::string 	m_sText;
        unsigned int	m_nX;
        unsigned int	m_nY;
//        int          	m_nFontID;
//        int          	m_nFontSize;
        GLColor         	m_Color;
};

} // SceneGraph


#endif
