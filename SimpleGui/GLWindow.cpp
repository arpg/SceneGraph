
#include <SimpleGui/GLWindow.h>

/////////////////////////////////////////////////////////////////////////////////
/// timer callback drives animation and forces drawing at 20 hz
void GLWindow::_Timer(void *userdata) {
    GLWindow *pWin = (GLWindow*)userdata;
    pWin->redraw();
    Fl::repeat_timeout( g_dFPS, _Timer, userdata );
}

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
// bird's eye view -- not really ortho!
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
void _SetupLighting()
{
    /////
    GLfloat ambientLight[]={0.1,0.1,0.1,1.0};                  // set ambient light parameters
    glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);

    GLfloat diffuseLight[]={0.8,0.8,0.8,1.0};                    // set diffuse light parameters
    glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);

    GLfloat specularLight[]={0.5,0.5,0.5,1.0};                   // set specular light parameters
    glLightfv(GL_LIGHT0,GL_SPECULAR,specularLight);

    GLfloat lightPos[]={ 0.0, 0.0, -100.0, 0.0};                       // set light position
    glLightfv( GL_LIGHT0, GL_POSITION, lightPos );

    GLfloat specularReflection[]={1.0,1.0,1.0,1.0};             // set specularity
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularReflection);
    glMateriali(GL_FRONT,GL_SHININESS,128);
    glEnable(GL_LIGHT0);                                           // activate light0
    glEnable(GL_LIGHTING);                                      // enable lighting
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);       // set light model
    glEnable(GL_COLOR_MATERIAL);                                  // activate material
    glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);                                     // normalize normal vectors

    // to check lighting
    glColor4f( 1, 1, 1, 1 );
    for( int y = -100; y < 100; y+=10 ){
        for( int x = -100; x < 100; x+=10 ){
            glPushMatrix();
            glTranslatef( x, y, 0 );
            glutSolidSphere( 2, 32, 32 );
            glPopMatrix();
        }
    }

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

    // call reistered Pre-draw frame listeners
    for( size_t ii = 0; ii < m_vPreRenderCallbacks.size(); ii++ ){
        (*m_vPreRenderCallbacks[ii])(this);
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

    _SetupLighting();


    //DrawSceneGraph();
    m_SceneGraph.draw();


    /*
    glTranslatef( 10, 0, 0 );
    glFrontFace(GL_CW); // wow, glutSolidTeapot has bugs!!!
    glutSolidTeapot(10);
    glFrontFace(GL_CCW);
    */

    // Draw console last
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    m_Console.draw();

    _UpdatePosUnderCursor();

    // now draw with picking
    _DoPicking();

    CheckForGLErrors();

    // call all the Post-draw frame listners
    for( size_t ii = 0; ii < m_vPostRenderCallbacks.size(); ii++ ){
        (*m_vPostRenderCallbacks[ii])(this); 
    }

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
Eigen::Vector3d GLWindow::GetNormalUnderCursor()
{
    return m_dNormalUnderCursor;
}

/////////////////////////////////////////////////////////////////////////////////
void GLWindow::_UpdatePosUnderCursor()
{
    /*
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
     */
    m_nMouseX = Fl::event_x();
    m_nMouseY = Fl::event_y();

    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY;//, winZ;
    //GLdouble posX1, posY1, posZ1;
    //GLdouble posX2, posY2, posZ2;

    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glGetIntegerv( GL_VIEWPORT, viewport );

    /*
       winX = (float)x;
       winY = (float)viewport[3] - (float)y;

       glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

       std::cout << "Z is " << winZ << std::endl;

       gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
     */

    const int nPatchWidth = 3;
    const int nPatchHeight = 3;

    float vPatch[ nPatchWidth ][ nPatchHeight ];
    winX = (float)m_nMouseX;
    winY = (float)viewport[3] - (float)m_nMouseY;
    glReadPixels( winX-nPatchWidth/2, int(winY)-nPatchHeight/2, nPatchWidth, nPatchHeight, GL_DEPTH_COMPONENT, GL_FLOAT, (float*)&vPatch[0][0] );

    Eigen::Vector3d Vec1;
    Eigen::Vector3d Vec2;
    Eigen::Vector3d VecA;
    Eigen::Vector3d VecB;

    int uL = 0, uM = nPatchWidth/2, uR = nPatchWidth - 1;
    int vT = 0, vM = nPatchHeight/2, vB = nPatchHeight - 1;
    gluUnProject( winX - nPatchWidth/2, winY, vPatch[vM][uL], modelview, projection, viewport, &Vec1[0], &Vec1[1], &Vec1[2] );
    gluUnProject( winX + nPatchWidth/2, winY, vPatch[vM][uR], modelview, projection, viewport, &Vec2[0], &Vec2[1], &Vec2[2] );
    VecA = Vec2 - Vec1;

    gluUnProject( winX, winY - nPatchHeight/2, vPatch[vT][uM], modelview, projection, viewport, &Vec1[0], &Vec1[1], &Vec1[2] );
    gluUnProject( winX, winY + nPatchHeight/2, vPatch[vB][uM], modelview, projection, viewport, &Vec2[0], &Vec2[1], &Vec2[2] );
    VecB = Vec2 - Vec1;

    m_dNormalUnderCursor = VecA.cross(VecB);
    m_dNormalUnderCursor = m_dNormalUnderCursor/m_dNormalUnderCursor.norm();

    gluUnProject( winX, winY, vPatch[vM][uM], modelview, projection, viewport, &m_dPosUnderCursor[0], &m_dPosUnderCursor[1], &m_dPosUnderCursor[2] );

}

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

///////////////////////////////////////////////////////////////////////////////////////////////
void GLWindow::AddPreRenderCallback( void(*f)(GLWindow*) )
{
    m_vPreRenderCallbacks.push_back( f );
}

///////////////////////////////////////////////////////////////////////////////////////////////
void GLWindow::AddPostRenderCallback( void(*f)(GLWindow*) )
{
    m_vPostRenderCallbacks.push_back( f );
}

////////////////////////////////////////////////////////////////////////////
int GLWindow::handle( int e )
{
    return SimpleDefaultEventHandler( e );
}

////////////////////////////////////////////////////////////////////////////
int GLWindow::SimpleDefaultEventHandler( int e )
{
    GLObject* pSelect = SelectedObject();
    if( pSelect && e == FL_PUSH ){
        //        printf("%s selected\n", pSelect->ObjectName() );
        m_eGuiMode = eSelect;
    }

    switch( m_eGuiMode ){
        case eConsole:
            {
                bool res = m_Console.handle( e );
                if( !m_Console.IsOpen() ){
                    m_eGuiMode = eFPSNav;
                }
                return res;
            }
        case eFPSNav:
            return HandleNavInput( e );
        case eSelect:
            return HandleSelectionInput( e );
    }

    return false;
}


////////////////////////////////////////////////////////////////////////////
/// generates first-person style control
int GLWindow::HandleNavInput( int e )
{
    switch( e ){
        case FL_PUSH:
            m_nMousePushX = Fl::event_x();
            m_nMousePushY = Fl::event_y();
            return 1;
        case FL_MOUSEWHEEL:
            {
                int dy = Fl::event_dy();
                // move camera along vector from CamPostion to CamTarget
                Eigen::Vector3d d = m_dCamTarget - m_dCamPosition;
                d = d / d.norm();
                if( Fl::event_ctrl() ) {
                    // up and down
                    m_dCamTarget   += -dy*m_dCamUp;
                    m_dCamPosition += -dy*m_dCamUp;
                } else {
                    m_dCamPosition = m_dCamPosition - dy*d;
                    m_dCamTarget = m_dCamPosition + d;
                }
                return 1;
            }
            break;
        case FL_DRAG:
            {
                int dx = Fl::event_x() - m_nMousePushX;
                int dy = Fl::event_y() - m_nMousePushY;
                if( Fl::event_button3() ) {
                    // "move" the world
                    double th = std::max( fabs(m_dCamPosition[2] / 100.0), 0.01 ); // move more or less depending on height
                    Eigen::Vector3d dForward = m_dCamTarget - m_dCamPosition;
                    dForward = dForward / dForward.norm();
                    Eigen::Vector3d dLR = dForward.cross(m_dCamUp);
                    // sideways
                    m_dCamTarget   += -dx * th*dLR;
                    m_dCamPosition += -dx * th*dLR;
                    // re-use "forward"
                    dForward = m_dCamUp.cross(dLR);
                    m_dCamTarget   += dy * th*dForward;
                    m_dCamPosition += dy * th*dForward;
                } else {
                    // just aim the camera -- we can write a better UI later...
                    Eigen::Vector3d dForward = m_dCamTarget - m_dCamPosition;
                    dForward = dForward / dForward.norm();

                    Eigen::Matrix3d dYaw   = GLCart2R( 0,        0, 0.001 * dx );
                    Eigen::Matrix3d dPitch = GLCart2R( 0, 0.001 * dy,        0 );

                    dForward = dYaw*dForward;
                    dForward = dPitch*dForward;

                    m_dCamTarget = m_dCamPosition + dForward;
                }
                m_nMousePushX = Fl::event_x();
                m_nMousePushY = Fl::event_y();
                return 1;
            }
            break;
        case FL_KEYBOARD:
            switch( Fl::event_key() ) {
                case '`':
                    m_Console.OpenConsole();
                    m_eGuiMode = eConsole;
                    break;
                case FL_F+1:
                    ResetCamera();
                    break;
                case FL_F+2:
                    CameraOrtho();
                    break;
            }
            return 1;
            break;

        case FL_KEYUP:
            switch( Fl::event_key() )
            {
                break;
            }
            break;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////
/// Handles object selection using result of glPicking
int GLWindow::HandleSelectionInput( int e )
{
    m_nMousePushX = Fl::event_x();
    m_nMousePushY = Fl::event_y();
    switch ( e ) {
        case FL_PUSH:
            {
                GLObject* pSelect = SelectedObject();
                if( pSelect ){
                    pSelect->select( SelectedId() );
                    m_pSelectedObject = pSelect;
                }
            }
            break;
        case FL_DRAG:
            if(m_pSelectedObject){
                m_pSelectedObject->drag();
            }
            break;
        case FL_RELEASE:
            m_eGuiMode = eFPSNav;
            if(m_pSelectedObject){
                m_pSelectedObject->release();
            }
    }
    return true;
}


