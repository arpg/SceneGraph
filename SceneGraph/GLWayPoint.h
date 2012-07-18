#ifndef _GL_WAYPOINT_
#define	_GL_WAYPOINT_

#include <SceneGraph/GLObject.h>
#include <SceneGraph/GLMesh.h>

namespace SceneGraph
{

const static int VELOCITY_MULTIPLIER = 2;

class GLWayPoint : public GLObject 
{
public:
    
    GLWayPoint()
        : GLObject("Waypoint")
    {
        m_bSelected = false;
        m_bDirty = false;

        m_dVelocity = 1.0;
        m_bPerceptable = false;
        m_wpScale = 0.25;

        // Set unique waypoint name
        static int wid = 0;
        char buf[128];
        snprintf(buf, 128, "WayPoint-%d", wid++);
        SetObjectName(buf);

        // Allocate extra picking id for front point
        m_nBaseId = AllocSelectionId();
        m_nFrontId = AllocSelectionId();
    }

//    bool Mouse(int button, const Eigen::Vector3d &win, const Eigen::Vector3d &obj, const Eigen::Vector3d& normal, bool pressed, int button_state, int pickId)
//    {
//        return false;
//    }

    bool MouseMotion(const Eigen::Vector3d& /*win*/, const Eigen::Vector3d &obj, const Eigen::Vector3d& normal, int /*button_state*/, int pickId)
    {
        m_bDirty = true; // flag for update
        Eigen::Matrix4d& T = m_T_po;

        if (pickId == m_nBaseId ) {
            Eigen::Vector3d d = -normal;
//            d << 0, 0, 1;
            Eigen::Vector3d f = T.block <3,1> (0, 0);
            Eigen::Vector3d r = d.cross(f);
            T.block<3,1>(0,3) = obj;
            T.block<3,1>(0,0) = f;
            T.block<3,1>(0,1) = r;
            T.block<3,1>(0,2) = d;
        }else if (pickId == m_nFrontId) {
            Eigen::Vector3d dir = obj - T.block<3,1>(0,3);
            Eigen::Vector3d nr  = (T.block<3,1>(0,2).cross(dir)).normalized();
            Eigen::Vector3d nf = nr.cross(T.block<3,1>(0,2));
            T.block<3,1>(0,0) = nf;
            T.block<3,1>(0,1) = nr;
            m_dVelocity = dir.norm()*VELOCITY_MULTIPLIER;
        }
        return true;
    }
        
    void DrawCanonicalObject() {
        glPushAttrib(GL_ENABLE_BIT);

        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);

        // draw velocity line
        glPushName(m_nFrontId);
        glColor4ub(255, 255, 255, 255);
        glBegin(GL_LINES);
        glVertex3d(m_dVelocity/VELOCITY_MULTIPLIER, 0, 0);
        glVertex3d(0, 0, 0);
        glEnd();
        glPopName();

        // draw axis
        glPushName(m_nBaseId);
        glBegin(GL_LINES);
        glColor4f(1, 0, 0, 1);
        glVertex3d(0, 0, 0);
        glVertex3d(m_wpScale, 0, 0);

        glColor4f(0, 1, 0, 1);
        glVertex3d(0, 0, 0);
        glVertex3d(0, m_wpScale, 0);

        glColor4f(0, 0, 1, 1);
        glVertex3d(0, 0, 0);
        glVertex3d(0, 0, m_wpScale);
        glEnd();

        if(m_bSelected) {
            glColor3ub(255, 0, 0);
        }else{
            glColor3ub(0, 255, 0);
        }
        
        // draw center point
        glPointSize(5);
        glBegin(GL_POINTS);
        glVertex3d(0, 0, 0);
        glEnd();
        glPopName();

        glPushName(m_nFrontId);
        // draw front velocity point
        glPointSize(5);
        glBegin(GL_POINTS);
        glVertex3d(m_dVelocity/VELOCITY_MULTIPLIER, 0, 0);
        glEnd();
        glPopName();
        
        // draw label
//            PushOrtho(WindowWidth(), WindowHeight());
//            glColor4f(1.0, 1.0, 1.0, 1.0);
//            gl_font(0, 8);
//            glRasterPos2f(m_nLabelPos(0), m_nLabelPos(1));
//            gl_draw(m_sLabel.c_str(), m_sLabel.length());
//            PopOrtho();

        glPopAttrib();
    }

    void SetDirty(bool bVal) { m_bDirty = bVal; }
    bool GetDirty() { return m_bDirty; }
    double GetVelocity() { return m_dVelocity; }
    Eigen::Matrix<double,5,1> GetPose5d(){
        Eigen::Matrix<double,5,1> pose5d;
        Eigen::Vector3d xVec = GetPose4x4_po().block<3,1>(0,0);
        pose5d << GetPose()[0], GetPose()[1], atan2(xVec[1],xVec[0]),m_dVelocity,0;
        return pose5d;
    }
    
private:
    bool            m_bSelected;
    bool            m_bDirty;
    double          m_dVelocity;
    float           m_wpScale;

    int             m_nBaseId;
    int             m_nFrontId;

//    double          m_dScale;
//    std::string     m_sLabel;
//    Eigen::Vector2i m_nLabelPos;
};

}

#endif
