#ifndef GLAXIS_H
#define GLAXIS_H

#include <SceneGraph/GLObject.h>

namespace SceneGraph
{

///////////////////////////////////////////////////////////////////////////////
class GLAxis : public GLObject
{
    public:

        ///////////////////////////////////////////////////////////////////////////
        GLAxis(const float axisSize = 1.0f, const bool bPretty = false)
            : GLObject("Axis"), m_fAxisScale(axisSize), m_bPretty(bPretty),
              m_pQuadric(0)
        {
            m_bPerceptable = false;
            m_aabb.SetZero();

            // this crashes if glu isn't initialized, which is often the case when
            // GlAxis is a member variable of some class. Potentially call this in Draw
            // if not initialized.
            if(bPretty){
                m_pQuadric = gluNewQuadric();
                gluQuadricNormals( m_pQuadric, GLU_SMOOTH );
                gluQuadricDrawStyle( m_pQuadric, GLU_FILL );
                gluQuadricTexture( m_pQuadric, GL_TRUE );
            }
        }

        ~GLAxis()
        {
            if( m_pQuadric ){
                gluDeleteQuadric( m_pQuadric );
            }
        }

        ///////////////////////////////////////////////////////////////////////////
        static inline void DrawAxis( float  fScale  = 1.0f )
        {
            // Draw axis
            glEnableClientState(GL_VERTEX_ARRAY);
            
            GLfloat axis[] = {
                0.0, 0.0, 0.0,
                fScale , 0, 0 };
            glColor4f(1, 0, 0, 1);
            glVertexPointer(3, GL_FLOAT, 0, axis);
            glDrawArrays(GL_LINE_STRIP, 0, 2);

            axis[3] = 0.0;
            axis[4] = fScale;
            glColor4f(0, 1, 0, 1);
            glVertexPointer(3, GL_FLOAT, 0, axis);
            glDrawArrays(GL_LINE_STRIP, 0, 2);

            axis[4] = 0.0;
            axis[5] = fScale;
            glColor4f(0, 0, 1, 1);
            glVertexPointer(3, GL_FLOAT, 0, axis);
            glDrawArrays(GL_LINE_STRIP, 0, 2);

            glDisableClientState(GL_VERTEX_ARRAY);
        }

        ///////////////////////////////////////////////////////////////////////////
        void DrawSolidAxis( float fScale = 1.0f )
        {
            // draw axis
            glScalef( fScale, fScale, fScale );

            // x
            glPushMatrix();
            glColor4f( 1, 0, 0, 1 );
            glRotatef( 90, 0, 1, 0 );
            gluCylinder( m_pQuadric, 0.04, 0.04, 1.0, 16, 16 );
            glTranslatef( 0, 0, 1 );
            gluCylinder( m_pQuadric, 0.07, 0.0, 0.2, 16, 16 );
            glPopMatrix();

            // y
            glPushMatrix();
            glColor4f( 0, 1, 0, 1 );
            glRotatef( -90, 1, 0, 0 );
            gluCylinder( m_pQuadric, 0.04, 0.04, 1.0, 16, 16 );
            glTranslatef( 0, 0, 1 );
            gluCylinder( m_pQuadric, 0.07, 0.0, 0.2, 16, 16 );
            glPopMatrix();

            // z
            glPushMatrix();
            glColor4f( 0, 0, 1, 1 );
            gluCylinder( m_pQuadric, 0.04, 0.04, 1.0, 16, 16 );
            glTranslatef( 0, 0, 1 );
            gluCylinder( m_pQuadric, 0.07, 0.0, 0.2, 16, 16 );
            glPopMatrix();

        }

        ///////////////////////////////////////////////////////////////////////////
        void DrawCanonicalObject()
        {            
            if(m_bPretty){
                DrawSolidAxis( m_fAxisScale );
            }else{
                DrawAxis(m_fAxisScale);
            }
        }

        void SetAxisSize(float  fScale ){ m_fAxisScale =  fScale ; }
        float GetAxisSize() { return m_fAxisScale; }

    protected:
        float m_fAxisScale;
        bool m_bPretty;
        GLUquadric*   m_pQuadric;
};

} // SceneGraph

#endif // GLAXIS_H
