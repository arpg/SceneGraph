#ifndef _GL_TEXT_H_
#define _GL_TEXT_H_

#include <SceneGraph/GLObject.h>
#include <SceneGraph/GLColor.h>
#include <SceneGraph/GLFont.h>

// TODO: Remove this and include GLUT directly here IFF GLUT support enabled.
//       Must use exactly same GLUT as Pangolin or there be errors.
#include <pangolin/pangolin.h>

#include <string>

namespace SceneGraph
{

#ifdef HAVE_GLES
// TODO: Implement these!
inline void glutBitmapCharacter(void */*font*/, int /*character*/) {}
inline void glRasterPos3f(GLfloat /*x*/, GLfloat /*y*/, GLfloat /*z*/) {}
inline int glutBitmapLength(void */*font*/, const unsigned char */*string*/) { return 0; }
#define GLUT_BITMAP_HELVETICA_10 ((void*)0)
#define GLUT_BITMAP_HELVETICA_12 ((void*)0)
#define GLUT_BITMAP_HELVETICA_18 ((void*)0)
#endif

static void* GLTextDefaultFont = GLUT_BITMAP_HELVETICA_10;

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
        for(const unsigned char* s = (unsigned char*)text.c_str(); *s != 0; ++s) {
            glutBitmapCharacter(font, *s);
        }
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
        m_Color.Apply();

        //            gl_font( m_nFontID, m_nFontSize );
        //            gl_draw( m_sText.c_str(), m_sText.length() );

        glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        Draw(m_sText, 0,0,0);
        glPopAttrib();
    }

    void SetText( const std::string& sText )
    {
        m_sText = sText;
    }

    void ClearText()
    {
        m_sText.clear();
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

private:
    void*           m_font;
    std::string 	m_sText;
    GLColor         m_Color;
};

} // SceneGraph


#endif
