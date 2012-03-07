#ifndef _GL_WAYPOINT_
#define	_GL_WAYPOINT_

#include "GLObject.h"

namespace Eigen{
    typedef Matrix<double,6,1> Vector6d;
	typedef Matrix<double,5,1> Vector5d;
}

extern std::vector<Eigen::Vector5d*>	gWayPoints;

class GLWayPoint : public GLObject
{
    public:

		GLWayPoint()
		{
            m_bInitDone = false;
            m_dPose = Eigen::Vector6d::Zero();
        }

        void select( unsigned int nId )
        {
            Eigen::Vector3d v = GetPosUnderCursor();
            m_nSelectedId = nId;
            if( nId == m_nBaseId ){
                m_dPose[0] = v[0];
                m_dPose[1] = v[1];
                m_dPose[2] = v[2];
            }
        }

        ///
        void drag()
        {
			char tBuff[100] = {};
            if( m_nSelectedId == m_nBaseId ){
                Eigen::Vector3d v = GetPosUnderCursor();
                m_dPose[0] = v[0];
                m_dPose[1] = v[1];
                m_dPose[2] = v[2];
				sprintf(tBuff, "(x: %.2f, y: %.2f)", v[0], v[1]);

            }
            if( m_nSelectedId == m_nFrontId ){
                Eigen::Vector3d v = GetPosUnderCursor();
                double dx = v[0] - m_dPose[0];
                double dy = v[1] - m_dPose[1];
                double th = atan2( dy, dx );
                m_dPose[5] = th;
                m_dVelocity = sqrt( dx*dx + dy*dy );
				sprintf(tBuff, "(a: %.2f, v: %.2f)", th, m_dVelocity);
            }
			m_sLabel = tBuff;
			m_nLabelPos = GetCursorPos();
			*gWayPoints[m_nWayPointId] << m_dPose[0], m_dPose[1], m_dPose[5], m_dVelocity, 0;
        }

		void release() {
			m_sLabel.clear();
		}


        void Init()
        {
            char buf[128];
            snprintf( buf, 128, "WayPoint-%d", m_nWayPointId );
            SetName( buf );
            m_nFrontId = AllocSelectionId();
            m_nBaseId = AllocSelectionId();
            m_bInitDone = true;
        }

        void draw()
		{
            if( !m_bInitDone ){
                Init();
            }
            glDisable( GL_DEPTH_TEST );
            glDisable(GL_CULL_FACE);
            glDisable(GL_LIGHTING);

            glColor4ub( 90, 90, 255, 128 );
			glPushMatrix();

            glTranslated( m_dPose[0], m_dPose[1], 0 );
            glRotated( 180*m_dPose[5]/M_PI, 0, 0, 1 );

			glBegin(GL_TRIANGLES);
            double d = 0.5;
			glVertex3d( d, 0, 0);
			glVertex3d( 0, -d/4, 0);
			glVertex3d( 0, d/4, 0);
			glEnd();

            glColor4ub( 255, 255, 255, 78 );
            glBegin( GL_LINES );
			glVertex3d( m_dVelocity, 0, 0);
			glVertex3d( 0, 0, 0);
            glEnd();

            // draw front point
            {
                glColor3ub( 0, 255, 0 );
                if( IsSelected( m_nFrontId ) ){
                   glColor3ub( 255, 0, 0 );
                    UnSelect( m_nFrontId );
                }

                glPushName( m_nFrontId ); // our GLObject ID
                glPointSize(5);
                glBegin( GL_POINTS );
                glVertex3d( m_dVelocity, 0, 0);
                glEnd();
                glPopName();
            }

            // draw back point
            {
                glColor3ub( 0, 255, 0 );
                if( IsSelected( m_nBaseId ) ){
                    glColor3ub( 255, 0, 0 );
                    UnSelect( m_nBaseId );
                }

                glPushName( m_nBaseId ); // our GLObject ID
                glPointSize(5);
                glBegin( GL_POINTS );
                glVertex3d( 0, 0, 0);
                glEnd();
                glPopName();
            }

			// draw label
			{
				PushOrtho( WindowWidth(), WindowHeight() );
				glColor4f( 1.0, 1.0, 1.0, 1.0 );
				gl_font( 0, 8 );
				glRasterPos2f( m_nLabelPos(0), m_nLabelPos(1) );
				gl_draw( m_sLabel.c_str(), m_sLabel.length() );
				PopOrtho();
			}

			glPopMatrix();
        }

		void SetWayPoint(
				const unsigned int nId,
                const double x,
                const double y,
                const double t,
                const double v
                )
		{
			m_nWayPointId = nId;
            m_dVelocity = v;
            m_dPose[0] = x;
            m_dPose[1] = y;
            m_dPose[5] = t;
		}

        void SetWayPoint(
				const unsigned int nId,
                Eigen::Vector5d vPt
                )
		{
			m_nWayPointId = nId;
            m_dVelocity = vPt[3];
            m_dPose[0] = vPt[0];
            m_dPose[1] = vPt[1];
            m_dPose[5] = vPt[2];
		}

    private:
		unsigned int                    m_nWayPointId;
        unsigned int                    m_nFrontId;
        unsigned int                    m_nBaseId;
        unsigned int                    m_nSelectedId;

		std::string						m_sLabel;
		Eigen::Vector2i					m_nLabelPos;

        Eigen::Vector6d                 m_dPose;
        double                          m_dVelocity;
        bool                            m_bInitDone;
};


#endif
