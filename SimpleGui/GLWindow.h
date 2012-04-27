#ifndef _GLWINDOW_H_
#define _GLWINDOW_H_

#include <SimpleGui/GLHelpers.h>

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>
#include <Fl/Enumerations.H>

#include <FLConsole/FLConsole.h>
#include <CVars/CVar.h>

#undef Success

#include <Eigen/Eigen>

#include <boost/thread.hpp>

#include <SimpleGui/GLHelpers.h>
#include <SimpleGui/GLObject.h>
#include <SimpleGui/GLSceneGraph.h>
#include <SimpleGui/GLSphereGrid.h>

const double g_dFPS = 1.0/50.0;

/////////////////////////////////////////////////////////////////////////////////
enum eGuiMode { eConsole, eFPSNav, eSelect };

/////////////////////////////////////////////////////////////////////////////////
/// Convert xyz,roll,pitch,yaw into a 4x4 homogeneous transform matrix
inline Eigen::Matrix4d GLCart2T(
        double x,
        double y,
        double z,
        double r,
        double p,
        double q
        )
{
    Eigen::Matrix4d T;
    // psi = roll, th = pitch, phi = yaw
    double cq, cp, cr, sq, sp, sr;
    cr = cos( r );
    cp = cos( p );
    cq = cos( q );

    sr = sin( r );
    sp = sin( p );
    sq = sin( q );

    T(0,0) = cp*cq;
    T(0,1) = -cr*sq+sr*sp*cq;
    T(0,2) = sr*sq+cr*sp*cq;

    T(1,0) = cp*sq;
    T(1,1) = cr*cq+sr*sp*sq;
    T(1,2) = -sr*cq+cr*sp*sq;

    T(2,0) = -sp;
    T(2,1) = sr*cp;
    T(2,2) = cr*cp;

    T(0,3) = x;
    T(1,3) = y;
    T(2,3) = z;
    T.row(3) = Eigen::Vector4d( 0.0, 0.0, 0.0, 1.0 );
    return T;
}


/////////////////////////////////////////////////////////////////////////////////
/// Convert roll, pitch and yaw into a rotation matrix
inline Eigen::Matrix3d GLCart2R(
        const double& r,
        const double& p,
        const double& q
        )
{
    Eigen::Matrix3d R;
    // psi = roll, th = pitch, phi = yaw
    double cq, cp, cr, sq, sp, sr;
    cr = cos( r );
    cp = cos( p );
    cq = cos( q );

    sr = sin( r );
    sp = sin( p );
    sq = sin( q );

    R(0,0) = cp*cq;
    R(0,1) = -cr*sq+sr*sp*cq;
    R(0,2) = sr*sq+cr*sp*cq;

    R(1,0) = cp*sq;
    R(1,1) = cr*cq+sr*sp*sq;
    R(1,2) = -sr*cq+cr*sp*sq;

    R(2,0) = -sp;
    R(2,1) = sr*cp;
    R(2,2) = cr*cp;
    return R;
}



/////////////////////////////////////////////////////////////////////////////////
/// FLTK OpenGL window.
class GLWindow : public Fl_Gl_Window, public boost::mutex
{
    /// timer callback drives animation and forces drawing at 20 hz
    static void _Timer(void *userdata);

    public:

    /// Constructor.
    GLWindow(int x,int y,int w,int h,const char *l=0);

    // position the camera at "origin"
    void ResetCamera();

    // bird's eye view -- bad bad bad, this isn't orhto at all
    void CameraOrtho( double dHeight = -50 );

    // Reset clear all registered objects
    void Reset();

    /// Init OpenGL
    void Init();
    bool IsSelected( unsigned int nId );
    GLObject* SelectedObject();
    unsigned int SelectedId();
    unsigned int AllocSelectionId( GLObject* pObj );
    void SetSelected( unsigned int nId );
    void UnSelect( unsigned int nId );

    /// Main function called by FLTK to draw the scene.
    void draw();

    // for mouse interaction
    Eigen::Vector2i GetCursorPos();
    Eigen::Vector3d GetPosUnderCursor();
    Eigen::Vector3d GetNormalUnderCursor();

    // Add new object to 2D layer scene graph
    void Add2DLayerToRoot( GLObject* pObj );

   // Add new object to scene graph
    void AddChildToRoot( GLObject* pObj );

    GLObject* GetObject( unsigned int nId );

    int Run();

    GLSceneGraph& SceneGraph();

    void AddPreRenderCallback( void(*pUserFunc)(GLWindow*,void*), void* pUserData = NULL );
    void AddPostRenderCallback( void(*pUserFunc)(GLWindow*,void*), void* pUserData = NULL );

    // event handlers -- these can serve as examples
    virtual int handle( int e ); // inherit and override if you want
    int SimpleDefaultEventHandler( int e ); // simple example handler
    int HandleNavInput( int e );
    int HandleSelectionInput( int e );

    void LookAt( float eyex, float eyey, float eyez,
            float targetx, float targety, float targetz,
            float upx, float upy, float upz );

    Eigen::Vector3d GetCamPosition() { return m_dCamPosition; }

    private:
    int                             m_nMouseX;
    int                             m_nMouseY;


    void  _RecursivelyInitObjects( GLObject* pObj );
    void _DoPicking();
    void _ProcessHits( unsigned int hits, GLuint buffer[] );
    void _UpdatePosUnderCursor();

    protected:
    int                             m_nMousePushX;
    int                             m_nMousePushY;

    Eigen::Vector3d                 m_dCamPosition;
    Eigen::Vector3d                 m_dCamTarget;
    Eigen::Vector3d                 m_dCamUp;

    std::map<int,GLObject*>			m_mSelectedObject;
    std::map<int,bool>              m_mSelected;
    unsigned int                    m_nSelectionId;

    FLConsole                       m_Console;

    Eigen::Vector3d                 m_dPosUnderCursor;
    Eigen::Vector3d                 m_dNormalUnderCursor;

    eGuiMode                        m_eGuiMode; // FPS nav, console or

    GLObject*                       m_pSelectedObject;

    GLSceneGraph                    m_SceneGraph;

    GLSceneGraph                    m_LayerSceneGraph;

    // little struct to hold user callback funciton and user data
    struct CallbackInfo
    {
        void(*m_pFuncPtr)(GLWindow*,void*);
        void*                      m_pUserData;
    };

    std::vector<CallbackInfo>        m_vPreRenderCallbacks;
    std::vector<CallbackInfo>        m_vPostRenderCallbacks;

    // for debugging...
    GLSphereGrid                     m_SphereGrid;
    bool                             m_bFullscreen;
    int                              m_nOldWidth;
    int                              m_nOldHeight;
};

#endif
