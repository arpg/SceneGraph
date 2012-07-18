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

    /////////////////////////////////
    // Static Utilities for drawing text
    /////////////////////////////////

    // These offer wrappers around e.g. glut so we can change the backend later
    // if we need to

    /// Render text at current glRasterPos
    static void Draw(const std::string& text, void* font = GLTextDefaultFont )
    {
        glutBitmapString(font,(unsigned char*)text.c_str());
    }

    /// Render text at (x,y,z)
    static void Draw(const std::string& text, float x, float y, float z = 0, void* font = GLTextDefaultFont)
    {
        glRasterPos3f(x,y,z);
        Draw(text, font);
    }

    /// Return width (in pixels) of text
    static int Width(const std::string& text, void* font = GLTextDefaultFont )
    {
        return glutBitmapLength(font, (unsigned char*)text.c_str());
    }

    /////////////////////////////////
    // GLText constructors
    /////////////////////////////////

    GLText(std::string text="", double x = 0, double y = 0, double z = 0)
        : GLObject("Text: " + text), m_font(GLTextDefaultFont), m_sText(text)
    {
        m_bPerceptable = false;
        SetPosition(x,y,z);
    }

    /////////////////////////////////
    // GLText member functions
    /////////////////////////////////

    void DrawCanonicalObject()
    {
        Eigen::Vector6d pos = GetPose();

        m_Color.Apply();

        //            gl_font( m_nFontID, m_nFontSize );
        //            gl_draw( m_sText.c_str(), m_sText.length() );

        glPushAttrib(GL_ENABLE_BIT);
//        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        Draw(m_sText, pos(0), pos(1), pos(2));
        glPopAttrib();
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

    void ClearText()
    {
        m_sText.clear();
    }

private:
    void*           m_font;

    std::string 	m_sText;
    //        int          	m_nFontID;
    //        int          	m_nFontSize;
    GLColor         	m_Color;
};

} // SceneGraph


#endif
