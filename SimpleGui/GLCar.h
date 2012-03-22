#ifndef _GL_CAR_
#define	_GL_CAR_

#include "GLObject.h"

class GLCar : public GLObject
{
    public:

		GLCar()
		{
			InitReset();
		}

		void InitReset()
		{
			m_Pose = Eigen::Matrix4d::Identity();
			m_fScale = 1.0;
            m_Color = GLColor();
		}

        void draw()
		{
			glPushMatrix();
			glMatrixMode( GL_MODELVIEW );
			glMultMatrixd(m_Pose.data());
            glColor4f( m_Color.r, m_Color.g, m_Color.b, m_Color.a );
			glBegin(GL_TRIANGLES);
			glVertex3d(m_fScale, 0, 0);
			glVertex3d(-m_fScale, -m_fScale/2, 0);
			glVertex3d(-m_fScale, m_fScale/2, 0);
			glEnd();
			glPopMatrix();
        }

		void SetPose( const Eigen::Matrix<double,6,1>& Pose )
		{
			m_Pose = _Cart2T(Pose);
		}

		void SetScale( float fScale )
		{
			m_fScale = fScale;
		}

        void SetColor( GLColor C )
		{
			m_Color = C;
		}

	private:
		Eigen::Matrix4d _Cart2T( const Eigen::Matrix<double,6,1>& Pose )
		{
			Eigen::Matrix4d T;
			// psi = roll, th = pitch, phi = yaw
			double cq, cp, cr, sq, sp, sr;
			cr = cos( Pose(3,0) );
			cp = cos( Pose(4,0) );
			cq = cos( Pose(5,0) );

			sr = sin( Pose(3,0) );
			sp = sin( Pose(4,0) );
			sq = sin( Pose(5,0) );

			T(0,0) = cp*cq;
			T(0,1) = -cr*sq+sr*sp*cq;
			T(0,2) = sr*sq+cr*sp*cq;

			T(1,0) = cp*sq;
			T(1,1) = cr*cq+sr*sp*sq;
			T(1,2) = -sr*cq+cr*sp*sq;

			T(2,0) = -sp;
			T(2,1) = sr*cp;
			T(2,2) = cr*cp;

			T(0,3) = Pose(0,0);
			T(1,3) = Pose(1,0);
			T(2,3) = Pose(2,0);
			T.row(3) = Eigen::Vector4d( 0.0, 0.0, 0.0, 1.0 );
			return T;
		}

    private:
		// centroid position + orientation
		Eigen::Matrix4d					m_Pose;

		// control scale of car
		float							m_fScale;
        GLColor                         m_Color;

};


#endif	/* _GL_CAR_ */
