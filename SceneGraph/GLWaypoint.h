#ifndef _GL_WAYPOINT_
#define	_GL_WAYPOINT_

#include <SceneGraph/GLObject.h>

namespace SceneGraph
{

const static float VELOCITY_MULTIPLIER = 1;

class GLWayPoint : public GLObject 
{
public:
    
    GLWayPoint()
        : GLObject("Waypoint")
    {
        SetAerial(false);
        m_bSelected = false;
        m_bDirty = false;
        m_bIsSelectable = true;
        m_bClampToPlane = false;
        m_bActive =false;
        m_bLocked = false;

        m_dVelocity = 1.0;
        m_bPerceptable = false;
        m_dScale = Eigen::Vector3d(0.25,0.25,0.25);

        // Set unique waypoint name
        static int wid = 0;
        char buf[128];
        snprintf(buf, 128, "WayPoint-%d", wid++);
        SetObjectName(buf);

        // Allocate extra picking id for front point
        m_nBaseId = AllocSelectionId();
        m_nFrontId = AllocSelectionId();
    }

    bool Mouse(int button, const Eigen::Vector3d& /*win*/, const Eigen::Vector3d& /*obj*/, const Eigen::Vector3d& /*normal*/, bool /*pressed*/, int /*button_state*/, int /*pickId*/)
    {
        if(button == MouseButtonLeft && m_bLocked == false){
            m_bPendingActive = true;
        }
        else if(button == MouseWheelUp && m_bLocked == false)
        {
            m_bDirty = true;
            m_dVelocity *= 1.01;
            return true;
        }else if(button == MouseWheelDown && m_bLocked == false)
        {
            m_bDirty = true;
            m_dVelocity *= 0.99;
            return true;
        }else {
            return false;
        }
    }

    bool MouseMotion(const Eigen::Vector3d& /*win*/, const Eigen::Vector3d &obj, const Eigen::Vector3d& normal, int /*button_state*/, int pickId)
    {
        m_bDirty = true; // flag for update
        Eigen::Matrix4d& T = m_T_po;

        // TODO: Convert p_w to local frame of reference
        Eigen::Vector3d p_w = obj;
        Eigen::Vector3d n_w = normal;

        if(m_bClampToPlane) {
            // project point obj onto plane
            const Eigen::Vector3d n = m_mClampPlaneN_p.head<3>();
            const double d = m_mClampPlaneN_p(3);
            p_w = p_w - ((p_w.dot(n)-d) * n);
            n_w = -n;
        }

        if (pickId == m_nBaseId  && m_bLocked == false) {
            Eigen::Vector3d d = -n_w;
//            d << 0, 0, 1;
            Eigen::Vector3d f = T.block <3,1> (0, 0);
            Eigen::Vector3d r = d.cross(f).normalized();
            f = r.cross(d).normalized();
            T.block<3,1>(0,3) = p_w;
            T.block<3,1>(0,0) = f;
            T.block<3,1>(0,1) = r;
            T.block<3,1>(0,2) = d;
        }else if (pickId == m_nFrontId && m_bLocked == false) {
            if(glutGetModifiers() & GLUT_ACTIVE_SHIFT){
                SetAerial(true);
            }
            Eigen::Vector3d dir = p_w - T.block<3,1>(0,3);
            Eigen::Vector3d nr  = (T.block<3,1>(0,2).cross(dir)).normalized();
            Eigen::Vector3d nf = nr.cross(T.block<3,1>(0,2)).normalized();
            T.block<3,1>(0,0) = nf;
            T.block<3,1>(0,1) = nr;
            m_dVelocity = dir.norm()*VELOCITY_MULTIPLIER;
        }
        return true;
    }
        
    void DrawCanonicalObject() {
        double multiplier = m_bActive || m_bLocked ? 1.0 : 0.5;
        glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);

        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        // TODO: loose this glDisable after we fix Nima's heightmap
        glDisable(GL_DEPTH_TEST);
        glDepthMask(false);

        const double velx = m_dVelocity/(m_dScale[0]*VELOCITY_MULTIPLIER);

        // draw velocity line
        glPushName(m_nFrontId);

        glColor4ub(255*multiplier, 255*multiplier, 255*multiplier, 255);

        glBegin(GL_LINES);
        glVertex3d(velx, 0, 0);
        glVertex3d(1, 0, 0);
        glEnd();
        glPopName();

        // draw axis
        glPushName(m_nBaseId);
        glBegin(GL_LINES);
        glColor4f(1*multiplier, 0, 0, 1);
        glVertex3d(0, 0, 0);
        glVertex3d(1, 0, 0);

        glColor4f(0, 1*multiplier, 0, 1);
        glVertex3d(0, 0, 0);
        glVertex3d(0, 1, 0);

        glColor4f(0, 0, 1*multiplier, 1);
        glVertex3d(0, 0, 0);
        glVertex3d(0, 0, 1);
        glEnd();

        if(m_bAerial) {

                glColor3ub(0, 0, 255*multiplier);

        }else{

                glColor3ub(0, 255*multiplier, 0);

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
        glVertex3d(velx, 0, 0);
        glEnd();
        glPopName();

        glPopAttrib();
    }

    void SetAerial(bool bVal) { m_bAerial = bVal; }
    bool GetAerial() const { return m_bAerial; }
    void SetDirty(bool bVal) { m_bDirty = bVal; }
    bool GetDirty() const { return m_bDirty; }

    double GetVelocity() const { return m_dVelocity; }
    void SetVelocity(double vel) { m_dVelocity = vel; }
    bool GetLocked() { return m_bLocked; }
    void SetLocked(bool bLocked){ m_bLocked = bLocked; }

    const Eigen::Matrix<double,5,1> GetPose5d() const
    {
        Eigen::Matrix<double,5,1> pose5d;
        Eigen::Vector3d xVec = GetPose4x4_po().block<3,1>(0,0);
        pose5d << GetPose4x4_po()(0,3), GetPose4x4_po()(1,3), atan2(xVec[1],xVec[0]),0,m_dVelocity;
        return pose5d;
    }

    void ClampToPlane(Eigen::Vector4d N_p)
    {
        m_mClampPlaneN_p = N_p;
        m_bClampToPlane = true;
    }

    bool            m_bPendingActive;
    bool            m_bActive;
    bool            m_bLocked;
    
private:


    bool            m_bAerial;
    bool            m_bSelected;
    bool            m_bDirty;
    double          m_dVelocity;

    int             m_nBaseId;
    int             m_nFrontId;

    bool            m_bClampToPlane;
    Eigen::Vector4d m_mClampPlaneN_p;

//    double          m_dScale;
//    std::string     m_sLabel;
//    Eigen::Vector2i m_nLabelPos;
};

}

#endif
