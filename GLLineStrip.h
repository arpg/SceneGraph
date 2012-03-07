#ifndef _GL_LINESTRIP_H_
#define _GL_LINESTRIP_H_

#include "GLObject.h"


class GLLineStrip : public GLObject
{
    public:

		GLLineStrip()
		{
			InitReset();
		}

        void InitReset()
        {
			m_Xref = 0;
			m_Yref = 0;
			m_Zref = 0;
			m_Color = GLColor();
        }

        void draw()
        {
            glBegin( GL_LINE_STRIP );
            glColor4f( m_Color.r, m_Color.g, m_Color.b, m_Color.a );
            for( unsigned int ii = 0; ii < m_vPts.size(); ii+=3 ) {
                  glVertex3d( m_vPts[ii] + m_Xref, m_vPts[ii+1] + m_Yref, m_vPts[ii+2] + m_Zref );
            }
            glEnd();
            /*
			glPointSize( 4 );
			glBegin( GL_POINTS );
			glColor3ub( 0, 255, 0 );
			int n = m_vPts.size()-3;
			glVertex3d( m_vPts[0] + m_Xref, m_vPts[1] + m_Yref, m_vPts[2] + m_Zref );
			glVertex3d( m_vPts[n] + m_Xref, m_vPts[n+1] + m_Yref, m_vPts[n+2] + m_Zref );
			glEnd();
            */
        }

        void SetPoint( Eigen::Vector3d Point )
		{
			m_vPts.push_back( Point(0) );
			m_vPts.push_back( Point(1) );
			m_vPts.push_back( Point(2) );
        }

		void SetPoint( double* Point )
		{
			m_vPts.push_back( Point[0] );
			m_vPts.push_back( Point[1] );
			m_vPts.push_back( Point[2] );
        }

		void SetPoint( double X = 0, double Y = 0, double Z = 0 )
		{
			m_vPts.push_back( X );
			m_vPts.push_back( Y );
			m_vPts.push_back( Z );
        }

		void SetPoints( std::vector<double>& vPts )
		{
			m_vPts = vPts;
		}

        void SetReference( unsigned int Xref, unsigned int Yref, unsigned int Zref )
		{
        	m_Xref = Xref;
        	m_Yref = Yref;
			m_Zref = Zref;
        }

        void ClearLines()
		{
        	m_vPts.clear();
        }

    private:
        std::vector< double >						m_vPts;
        unsigned int								m_Xref;
        unsigned int			 				    m_Yref;
		unsigned int								m_Zref;
        GLColor         				   				m_Color;
};


#endif
