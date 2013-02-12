#ifndef _GL_LINESTRIP_H_
#define _GL_LINESTRIP_H_

#include <SceneGraph/GLObject.h>
#include <SceneGraph/GLColor.h>

namespace SceneGraph
{

class GLLineStrip : public GLObject
{
    public:

		GLLineStrip()
            :GLObject("LineStrip")
		{
            m_bPerceptable = false;
		}

        void InitReset()
        {
			m_Color = GLColor();
        }

        void DrawCanonicalObject()
        {
            glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);

//            glDepthMask(false);

            m_Color.Apply();
            glBegin( GL_LINE_STRIP );
            for( unsigned int ii = 0; ii < m_vPts.size(); ii+=3 ) {
                  glVertex3d( m_vPts[ii], m_vPts[ii+1], m_vPts[ii+2] );
            }
            glEnd();
            glPopAttrib();
        }

        void SetPoint( Eigen::Vector3d Point )
		{
			m_vPts.push_back( Point(0) );
			m_vPts.push_back( Point(1) );
			m_vPts.push_back( Point(2) );
        }

        void SetPoint( double Point[3] )
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

		void SetPoints( const std::vector<double>& vPts )
		{
			m_vPts = vPts;
		}

        void SetPointsFromTrajectory( const Eigen::Vector6dAlignedVec& vPts )
		{
            m_vPts.clear();
            for( size_t ii = 0; ii < vPts.size(); ii++ ){
                m_vPts.push_back( vPts[ii][0] );
                m_vPts.push_back( vPts[ii][1] );
                m_vPts.push_back( vPts[ii][2] );
            }
		}

        SCENEGRAPH_DEPRECATED
        void SetReference( unsigned int Xref, unsigned int Yref, unsigned int Zref )
		{
            m_T_po(0,3) = Xref;
            m_T_po(1,3) = Yref;
            m_T_po(2,3) = Zref;
        }

        void ClearLines()
		{
        	m_vPts.clear();
        }

        void SetColor( const GLColor& c )
        {
            m_Color = c;
        } 

    private:
        std::vector< double >						m_vPts;
        GLColor         				   			m_Color;
};

}

#endif
