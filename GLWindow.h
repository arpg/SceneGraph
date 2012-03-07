#ifndef _GLWINDOW_H_
#define _GLWINDOW_H_

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>

#include <FLConsole/FLConsole.h>
#include <CVars/CVar.h>

#include <Eigen/Eigen>

#include <boost/thread.hpp>

#include "GLHelpers.h"
#include "GLObject.h"


const double g_dFPS = 1.0/20.0;


enum eGuiMode { eConsole, eFPSNav, eSelect };

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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



/// FLTK OpenGL window.
class GLWindow : public Fl_Gl_Window, public boost::mutex
{
    /// timer callback drives animation and forces drawing at 20 hz
    static void _Timer(void *userdata) {
        GLWindow *pWin = (GLWindow*)userdata;
        pWin->Update();
		pWin->redraw();
        Fl::repeat_timeout( g_dFPS, _Timer, userdata );
    }

    public:

    void DrawSceneGraph()
    {
        for( size_t ii = 0; ii < m_vSceneGraph.size(); ii++ ){
            if( m_vSceneGraph[ii]->IsVisible() ){
                if( valid() ){
                    lock();
                    //            printf("Drawing %s\n", m_vSceneGraph[ii]->ObjectName() );
                    CheckForGLErrors();
                    m_vSceneGraph[ii]->draw();
                    unlock();
                }
            }
            else{
                //                printf("NOT Drawing %s\n", m_vSceneGraph[ii]->ObjectName() );
            }
        }
    }

    /// Constructor.
    GLWindow(int x,int y,int w,int h,const char *l=0) : Fl_Gl_Window(x,y,w,h,l)
    {
        m_pSelectedObject = NULL;
        m_eGuiMode = eFPSNav;
        ResetCamera();
        Fl::add_timeout( g_dFPS, _Timer, (void*)this );
		end();
		resizable( this );
		show();
    }

    // position the camera at "origin"
    void ResetCamera()
    {
        m_dCamPosition[0] = -5;
        m_dCamPosition[1] = -5;
        m_dCamPosition[2] = -5;

        m_dCamTarget[0] = 100;
        m_dCamTarget[1] = 100;
        m_dCamTarget[2] = 0;

        m_dCamUp[0] = 0;
        m_dCamUp[1] = 0;
        m_dCamUp[2] = -1;
    }

	// bird's eye view
	void CameraOrtho()
    {
        m_dCamPosition[0] = 0;
        m_dCamPosition[1] = 0;
        m_dCamPosition[2] = -50;

        m_dCamTarget[0] = 0;
        m_dCamTarget[1] = 0;
        m_dCamTarget[2] = 0;

        m_dCamUp[0] = 1;
        m_dCamUp[1] = 0;
        m_dCamUp[2] = 0;
    }

    // Reset clear all registered objects
    void Reset()
    {
        ResetCamera();
        m_vSceneGraph.clear();
    }

    /// Init OpenGL
    void Init()
    {
        // OpenGL settings
        glShadeModel( GL_SMOOTH );
        glEnable( GL_DEPTH_TEST );
        glDepthFunc( GL_LEQUAL );
        glEnable(GL_LINE_SMOOTH);
        glClearColor( 0.0, 0.0, 0.0, 1.0 );
    }

    bool IsSelected( unsigned int nId )
    {
        std::map<int,bool>::iterator it = m_mSelected.find( nId );
        if( it == m_mSelected.end() ){
            return false;
        }
        return m_mSelected[nId];
    }

	GLObject* SelectedObject()
    {
		std::map<int,bool>::iterator it;
		for( it = m_mSelected.begin(); it != m_mSelected.end(); it++ ) {
			if( it->second == true ) {
				return m_mSelectedObject[it->first];
			}
		}
		return NULL;
	}

    unsigned int SelectedId()
    {
		std::map<int,bool>::iterator it;
		for( it = m_mSelected.begin(); it != m_mSelected.end(); it++ ) {
			if( it->second == true ) {
				return it->first;
			}
		}
		return INT_MAX;
	}

    unsigned int AllocSelectionId( GLObject* pObj )
    {
        int nId = m_nSelectionId++;
		m_mSelectedObject[nId] = pObj;
        return nId;
    }


    void SetSelected( unsigned int nId )
    {
        m_mSelected[ nId ] = true;
    }

    void UnSelect( unsigned int nId )
    {
        m_mSelected[ nId ] = false;
    }

    /// Main function called by FLTK to draw the scene.
    void draw() {

        if( !context() ) {
            return;
        }

        CheckForGLErrors();

        // handle reshaped viewport
        if ( !valid() ) {
            ReshapeViewport( w(), h() );
        }

        // Initialization
        static bool bInitialized = false;
        if ( !bInitialized || !context() ) {
            bInitialized = true;
            Init();
            return;
        }

        // Clear
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        // Aim camera
        Eigen::Vector3d dPos    = m_dCamPosition;
        Eigen::Vector3d dTarget = m_dCamTarget;
        Eigen::Vector3d dUp     = m_dCamUp;

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt( dPos[0], dPos[1], dPos[2],
                   dTarget[0], dTarget[1], dTarget[2], dUp[0], dUp[1], dUp[2] );

        DrawSceneGraph();

        // Draw console last
        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        m_Console.draw();

        _UpdatePosUnderCursor();

        // now draw with picking
        _DoPicking();

        CheckForGLErrors();
    }


    /////////////////////////////////////////////////////////////////////////////////
    void _DoPicking()
    {
        GLint viewport[4];
        GLdouble projection[16];
        glGetIntegerv( GL_VIEWPORT, viewport );
        glGetDoublev( GL_PROJECTION_MATRIX, projection );
        double x = Fl::event_x();
        double y = Fl::event_y();
        unsigned int nBufSize = 64;
        GLuint vSelectBuf[64];
        glSelectBuffer( nBufSize, vSelectBuf );
        (void) glRenderMode( GL_SELECT );
        glMatrixMode( GL_PROJECTION );
        glPushMatrix();
        glLoadIdentity();
        gluPickMatrix( x, viewport[3] - y, 15.0, 15.0, viewport );
        glMultMatrixd( projection );
        glMatrixMode(GL_MODELVIEW );
        glInitNames();

        DrawSceneGraph();

        glMatrixMode( GL_PROJECTION );
        glPopMatrix();
        glMatrixMode( GL_MODELVIEW );
        glFlush();
        GLint nHits = glRenderMode( GL_RENDER );
        ProcessHits( nHits, vSelectBuf );

    }

    /////////////////////////////////////////////////////////////////////////////////
    void ProcessHits( GLint hits, GLuint buffer[] )
    {
        unsigned int i, j;
        GLuint names, *ptr;
        ptr = (GLuint *) buffer;
        for (i = 0; i < hits; i++) { //  for each hit
            names = *ptr;
            ptr++;
            float depth1 = (float) *ptr/0x7fffffff;
            ptr++;
            float depth2 = (float) *ptr/0x7fffffff;
            ptr++;
            for (j = 0; j < names; j++) {     //  for each name
                int nId = *ptr;
                SetSelected( nId );

                GLObject* pObj = SelectedObject();

//                printf("set %s as selected\n", pObj->ObjectName() );

                ptr++;
            }
        }
    }

	/////////////////////////////////////////////////////////////////////////////////
    Eigen::Vector2i GetCursorPos()
    {
		Eigen::Vector2i Pos;
		Pos(0) = m_nMousePushX;
		Pos(1) = m_nMousePushY;
        return Pos;
    }

    /////////////////////////////////////////////////////////////////////////////////
    Eigen::Vector3d GetPosUnderCursor()
    {
        return m_dPosUnderCursor;
    }

    /////////////////////////////////////////////////////////////////////////////////
    void _UpdatePosUnderCursor()
    {
        int x = Fl::event_x();
        int y = Fl::event_y();
        GLint viewport[4];
        GLdouble modelview[16];
        GLdouble projection[16];
        GLfloat winX, winY, winZ;
        GLdouble posX, posY, posZ;

        glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
        glGetDoublev( GL_PROJECTION_MATRIX, projection );
        glGetIntegerv( GL_VIEWPORT, viewport );

        winX = (float)x;
        winY = (float)viewport[3] - (float)y;
        glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );


        gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

        m_dPosUnderCursor << posX, posY, posZ;
    }

    // handle input events
    int handle( int e );

	// function called to update objects
	void Update();

    // function handles input when in normal FPS mode
    int HandleNavInput( int e );

    // function handles input when in selection mode
    int HandleSelectionInput( int e );

	// Add new object to scene graph
    void RegisterObject( GLObject* pObj )
    {
        // lock scene graph
		lock();
        pObj->init( this );
        int nId = m_vSceneGraph.size();
        m_vSceneGraph.push_back( pObj );
        pObj->SetId(nId);
        // unlock scene graph
		unlock();
    }

    GLObject* GetObject( unsigned int nId )
    {
        printf("m_vSceneGraph[%d] = %d\n", nId, m_vSceneGraph[nId]->Id() );
        return m_vSceneGraph[nId];
    }

    private:
        int                             m_nMousePushX;
        int                             m_nMousePushY;

        Eigen::Vector3d                 m_dCamPosition;
        Eigen::Vector3d                 m_dCamTarget;
        Eigen::Vector3d                 m_dCamUp;

		std::map<int,GLObject*>			m_mSelectedObject;
        std::map<int,bool>              m_mSelected;
        unsigned int                    m_nSelectionId;

        FLConsoleInstance 	   			m_Console;
        std::vector<GLObject*>  		m_vSceneGraph;

        Eigen::Vector3d                 m_dPosUnderCursor;

        eGuiMode                        m_eGuiMode;

        std::vector<float>              m_vDepthBuffer;

        GLObject*                       m_pSelectedObject;
};

///////////////////////////////////////////////////////////////////////////////////////////////
/// Certain GLObject code has to be implemented here, after GLWindow.
///////////////////////////////////////////////////////////////////////////////////////////////

inline int GLObject::WindowWidth()
{
    return m_pWin->w();
}

inline int GLObject::WindowHeight()
{
    return m_pWin->h();
}

inline bool GLObject::IsSelected( unsigned int nId )
{
    return m_pWin->IsSelected( nId );
}

inline void GLObject::UnSelect( unsigned int nId )
{
    m_pWin->UnSelect( nId );
}

inline unsigned int GLObject::AllocSelectionId()
{
	unsigned int nId =  m_pWin->AllocSelectionId( this );
    return nId;
}

inline Eigen::Vector3d GLObject::GetPosUnderCursor()
{
	return m_pWin->GetPosUnderCursor();
}

inline Eigen::Vector2i GLObject::GetCursorPos()
{
	return m_pWin->GetCursorPos();
}

inline bool GLObject::valid()
{
    if( m_pWin && m_pWin->valid() ) {
        return true;
    }
    return false;
}

#endif
