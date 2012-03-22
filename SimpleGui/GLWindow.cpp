
#include "GLWindow.h"

/////////////////////////////////////////////////////////////////////////////////
/// timer callback drives animation and forces drawing at 20 hz
void GLWindow::_Timer(void *userdata) {
    GLWindow *pWin = (GLWindow*)userdata;
    pWin->redraw();
    Fl::repeat_timeout( g_dFPS, _Timer, userdata );
}

/*
/////////////////////////////////////////////////////////////////////////////////
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
 */

/////////////////////////////////////////////////////////////////////////////////
/// Constructor.
GLWindow::GLWindow(int x,int y,int w,int h,const char *l ) : Fl_Gl_Window(x,y,w,h,l)
{
    m_SceneGraph.InitWindowPtr(  this );
    m_pSelectedObject = NULL;
    m_eGuiMode = eFPSNav;
    ResetCamera();
    Fl::add_timeout( g_dFPS, _Timer, (void*)this );
    end();
    resizable( this );
    show();
}

/////////////////////////////////////////////////////////////////////////////////
// position the camera at "origin"
void GLWindow::ResetCamera()
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

/////////////////////////////////////////////////////////////////////////////////
// bird's eye view
void GLWindow::CameraOrtho()
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

/////////////////////////////////////////////////////////////////////////////////
// Reset clear all registered objects
void GLWindow::Reset()
{
    ResetCamera();
    m_SceneGraph.Reset();
}

/////////////////////////////////////////////////////////////////////////////////
/// Init OpenGL
void GLWindow::Init()
{
    // OpenGL settings
    glShadeModel( GL_SMOOTH );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    glEnable(GL_LINE_SMOOTH);
    glClearColor( 0.0, 0.0, 0.0, 1.0 );
}

/////////////////////////////////////////////////////////////////////////////////
bool GLWindow::IsSelected( unsigned int nId )
{
    std::map<int,bool>::iterator it = m_mSelected.find( nId );
    if( it == m_mSelected.end() ){
        return false;
    }
    return m_mSelected[nId];
}

/////////////////////////////////////////////////////////////////////////////////
GLObject* GLWindow::SelectedObject()
{
    std::map<int,bool>::iterator it;
    for( it = m_mSelected.begin(); it != m_mSelected.end(); it++ ) {
        if( it->second == true ) {
            return m_mSelectedObject[it->first];
        }
    }
    return NULL;
}

/////////////////////////////////////////////////////////////////////////////////
unsigned int GLWindow::SelectedId()
{
    std::map<int,bool>::iterator it;
    for( it = m_mSelected.begin(); it != m_mSelected.end(); it++ ) {
        if( it->second == true ) {
            return it->first;
        }
    }
    return INT_MAX;
}

/////////////////////////////////////////////////////////////////////////////////
unsigned int GLWindow::AllocSelectionId( GLObject* pObj )
{
    int nId = m_nSelectionId++;
    m_mSelectedObject[nId] = pObj;
    return nId;
}


/////////////////////////////////////////////////////////////////////////////////
void GLWindow::SetSelected( unsigned int nId )
{
    m_mSelected[ nId ] = true;
}

/////////////////////////////////////////////////////////////////////////////////
void GLWindow::UnSelect( unsigned int nId )
{
    m_mSelected[ nId ] = false;
}

/////////////////////////////////////////////////////////////////////////////////
/// Main function called by FLTK to draw the scene.
void GLWindow::draw() {

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

    //DrawSceneGraph();
    m_SceneGraph.draw();

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
void GLWindow::_DoPicking()
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

    //DrawSceneGraph();
    m_SceneGraph.draw();

    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
    glFlush();
    GLint nHits = glRenderMode( GL_RENDER );
    _ProcessHits( nHits, vSelectBuf );

}

/////////////////////////////////////////////////////////////////////////////////
void GLWindow::_ProcessHits( unsigned int hits, GLuint buffer[] )
{
    unsigned int ii, jj;
    GLuint names, *ptr;
    ptr = (GLuint *) buffer;
    for( ii = 0; ii < hits; ii++ ) { //  for each hit
        names = *ptr;
        ptr++;
        //float depth1 = (float) *ptr/0x7fffffff;
        ptr++;
        //float depth2 = (float) *ptr/0x7fffffff;
        ptr++;
        for( jj = 0; jj < names; jj++ ) {     //  for each name
            int nId = *ptr;
            SetSelected( nId );

//            GLObject* pObj = SelectedObject();
            //                printf("set %s as selected\n", pObj->ObjectName() );

            ptr++;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////
Eigen::Vector2i GLWindow::GetCursorPos()
{
    Eigen::Vector2i Pos;
    Pos(0) = m_nMousePushX;
    Pos(1) = m_nMousePushY;
    return Pos;
}

/////////////////////////////////////////////////////////////////////////////////
Eigen::Vector3d GLWindow::GetPosUnderCursor()
{
    return m_dPosUnderCursor;
}

/////////////////////////////////////////////////////////////////////////////////
void GLWindow::_UpdatePosUnderCursor()
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

///////////////////////////////////////////////////////////////////////////////////////////////
// handle input events
//int GLWindow::handle( int e );

///////////////////////////////////////////////////////////////////////////////////////////////
// function called to update objects
//void GLWindow::Update();

///////////////////////////////////////////////////////////////////////////////////////////////
// function handles input when in normal FPS mode
//int GLWindow::HandleNavInput( int e );

///////////////////////////////////////////////////////////////////////////////////////////////
// function handles input when in selection mode
//int GLWindow::HandleSelectionInput( int e );

///////////////////////////////////////////////////////////////////////////////////////////////
// Add new object to scene graph
void GLWindow::AddChildToRoot( GLObject* pObj )
{
    lock(); // lock scene graph
    pObj->InitWindowPtr( this );
    m_SceneGraph.AddChild( pObj );
    unlock(); // unlock scene graph
}

///////////////////////////////////////////////////////////////////////////////////////////////
GLObject* GLWindow::GetObject( unsigned int nId )
{
    return m_SceneGraph.GetObject(nId);
    //        printf("m_SceneGraph[%d] = %d\n", nId, m_vSceneGraph[nId]->Id() );
    //        return m_vSceneGraph[nId];
}

///////////////////////////////////////////////////////////////////////////////////////////////
int GLWindow::Run()
{
    return( Fl::run() );
}

///////////////////////////////////////////////////////////////////////////////////////////////
GLSceneGraph& GLWindow::SceneGraph()
{
    return m_SceneGraph;
}

