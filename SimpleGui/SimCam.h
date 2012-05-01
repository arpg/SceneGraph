#ifndef _SIMCAM_H_
#define _SIMCAM_H_

#include <SimpleGui/FBO.h>
#include <SimpleGui/GLSceneGraph.h>
#include <SimpleGui/GLSLHelpers.h>
#include <SimpleGui/GLCVars.h>

#include <string>

class GLSimCam;

/// type of rendering modes we support
enum eSimCamType{
    eSimCamLuminance = 1,
    eSimCamRGB = 2,
    eSimCamDepth = 4,
    eSimCamNormals = 8
};


class SimCamMode
{
    friend class GLSimCam; // only SimCam can have a "SimCamMode"
    private:
        SimCamMode( GLSimCam& sc, eSimCamType eCamType ); // private, only SimCam can make one...
        ~SimCamMode();
        void Init( bool shader, GLuint sp, GLenum format, GLenum type );
        void RenderToTexture();
        void* Capture();
        GLuint Texture();

    private:
        static GLenum getNextAttachmentIndex();
        static int getNextCTId();
        static void _ShaderVisitor( GLObject* pObj );

        void PboRead();
	// void PboReadDepth();
        void PboInit( int format, int type );

    private:

        GLSimCam&       m_SimCam; // required -- can not construct a SimCamMode wo a SimCam...
        bool            m_bHasShader;

        eSimCamType        m_eCamType;
        GLuint             m_nShaderProgram;
        int                m_nPboIndex;
        int                m_nColorTextureId;
        GLenum             m_eAttachmentIndex;
        void*           m_pBuffer; // James, consider using std::vector to automate memory management
        GLuint*            m_pPboIds;
	GLenum             m_eFormat;
	GLenum             m_eType;
        int                m_nDataSize;
};

class GLSimCam
{
    public:

        /////////////////////////////////////////////////////////////////////////////////////////
        GLSimCam()
        {
            m_bInitDone = false;
            m_pSceneGraph = NULL;
            m_pFbo = FBO::Instance();
            m_pRGBMode = NULL;
            m_pDepthMode = NULL;
            m_pNormalsMode = NULL;
            m_bOrthoCam = false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        void SetOrtho( float dWidth, float dHeight )
        {
            m_fOrthoTop    = -dHeight / 2.0; // -z dir
            m_fOrthoBottom = dHeight / 2.0; // +z dir
            m_fOrthoLeft   = -dWidth / 2.0; // -y dir
            m_fOrthoRight  = dWidth / 2.0; // +y dir
            m_bOrthoCam    = true;
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        void UnSetOrtho()
        {
            m_bOrthoCam = false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        /// Store pose as a roboitcs frame pose
        void SetPose( const Eigen::Matrix4d& dPose )
        {
            m_dPose = dPose;
        }

        ///////////////////////////////////////////////////////////////////////////////
        Eigen::Matrix4d& GetPoseRef()
        {
            return m_dPose;
        }

        ///////////////////////////////////////////////////////////////////////////////
        void SetIntrinsics( const Eigen::Matrix3d& dK )
        {
            m_dK = dK;
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        /// Init shaders. Given K matrix, compute the corresponding GL_PROJECTION_MATRIX.
        void Init(
                GLSceneGraph* pSceneGraph,       //< Input: pointer to scene graph we should render
                const Eigen::Matrix4d dPose,     //< Input: initial camera pose
                const Eigen::Matrix3d dK,        //< Input: computer vision K matrix
                const unsigned int nSensorWidth, //< Input: sensor width in pixels
                const unsigned int nSensorHeight,//< Input: sensor height in pixels
                int nModes = eSimCamRGB,         //< Input:
                float fNear = 1,                //< Input: opengl near clipping plane
                float fFar = 100                //< Input: opengl far clipping plane
                )
        {
            m_pSceneGraph = pSceneGraph;
            m_nSensorWidth = nSensorWidth;
            m_nSensorHeight = nSensorHeight;
            m_fNear = fNear;
            m_fFar = fFar;
            m_dK = dK;
            m_dPose = dPose;

            std::string sDepthVertShader =
                "varying float depth;\n"
                "void main(void)\n"
                "{\n"
                "    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;\n"
                "    depth = -(gl_ModelViewMatrix * gl_Vertex).z;\n"
                "}\n";

            std::string sDepthFragShader =
                "varying float depth;\n"
                "void main(void)\n"
                "{\n"
                "    float A = gl_ProjectionMatrix[2].z;\n"
                "    float B = gl_ProjectionMatrix[3].z;\n"
                "    float zNear = - B / (1.0 - A);\n"
                "    float zFar  =   B / (1.0 + A);\n"
                "    float depthN = (depth - zNear)/(zFar - zNear);  // scale to a value in [0, 1]\n"
                "    gl_FragColor[0] = depthN;\n"
                "}\n";
	    InitShaders( sDepthVertShader, sDepthFragShader, m_nDepthShaderProgram );

	    /* Helpful for quick testing of shaders, don't need to recompile if loaded from file
	    if ( LoadShaders( "Depth.vert", "Depth.frag", m_nDepthShaderProgram ) == false) {
	        fprintf(stderr, "Failed to load the Depth shader.");
		} */

            std::string sNormalVertShader =
                "varying vec3 normal;\n"
                "void main()\n"
                "{\n"
                "    gl_Position = ftransform();\n"
                "    normal = gl_NormalMatrix * gl_Normal;\n"
                "}\n";

            std::string sNormalFragShader =
                "varying vec3 normal;\n"
                "void main()\n"
                "{\n"
                "   gl_FragColor.rgba = vec4( normal, 1.0 );\n"
                "}\n";

            InitShaders( sNormalVertShader, sNormalFragShader, m_nNormalShaderProgram );

            if( nModes & eSimCamRGB ){
                m_pRGBMode = new SimCamMode( *this, eSimCamRGB );
                m_pRGBMode->Init( false, 0, GL_RGB, GL_UNSIGNED_BYTE );
            }
            if( nModes & eSimCamDepth ){
                m_pDepthMode = new SimCamMode( *this, eSimCamDepth );
		                m_pDepthMode->Init( true, m_nDepthShaderProgram, GL_LUMINANCE, GL_FLOAT );
                //m_pDepthMode->Init( true, m_nDepthShaderProgram, GL_RGB, GL_UNSIGNED_BYTE );

            }
            if( nModes & eSimCamNormals ){
                m_pNormalsMode = new SimCamMode( *this, eSimCamNormals );
                m_pNormalsMode->Init( true, m_nNormalShaderProgram, GL_RGB, GL_UNSIGNED_BYTE );
            }

            // May have change this for multiple SimCams (since there is only one FBO)
            // setup our frame buffer object for off screen rendering
            m_pFbo->Init( nSensorWidth, nSensorHeight );

            m_bInitDone = true;
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        GLint RGBTexture()
        {
            if( m_pRGBMode ){
                return m_pRGBMode->Texture();
            }
            return -1;
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        GLint DepthTexture()
        {
            if( m_pDepthMode ){
                return m_pDepthMode->Texture();
            }
            return -1;
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        GLint NormalsTexture()
        {
            if( m_pNormalsMode ){
                return m_pNormalsMode->Texture();
            }
            return -1;
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        bool CaptureRGB( std::vector<unsigned char>& vPixelData )
        {
            if( m_pRGBMode ){
                if( vPixelData.size() < ImageWidth()*ImageHeight()*3 ){
                    vPixelData.resize( ImageWidth()*ImageHeight()*3 );
                }
                // TODO Capture here should do the PboRead and fill vPixelData...
		// NB: Having Capture do a PboRead may cause probelems if the render state is not
		// set correctly. Right now, PboRead is only called while in middle of the regular
		// render loop. - JamesII
                memcpy( &vPixelData[0], m_pRGBMode->Capture(), ImageWidth()*ImageHeight()*3);
                return true;
            }
            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        bool CaptureRGB( void* DataPtr )
        {
            if( m_pRGBMode ){
                // TODO Capture here should do the PboRead and fill vPixelData...
		// NB: Having Capture do a PboRead may cause probelems if the render state is not
		// set correctly. Right now, PboRead is only called while in middle of the regular
		// render loop. - JamesII
                memcpy( DataPtr, m_pRGBMode->Capture(), ImageWidth()*ImageHeight()*3);
                return true;
            }
            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        bool CaptureDepth( std::vector<float>& vPixelData )
        {
            if( m_pDepthMode ){
                if( vPixelData.size() < ImageWidth()*ImageHeight()*sizeof(float) ){
                    vPixelData.resize( ImageWidth()*ImageHeight()*sizeof(float) );
                }
                // TODO Capture here should do the PboRead and fill vPixelData...
                memcpy( &vPixelData[0], m_pDepthMode->Capture(),
                        ImageWidth()*ImageHeight()*sizeof(float));
                return true;
            }
            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        bool CaptureNormals( std::vector<unsigned char>& vPixelData )
        {
            if( m_pNormalsMode ){
                if( vPixelData.size() < ImageWidth()*ImageHeight()*3 ){
                    vPixelData.resize( ImageWidth()*ImageHeight()*3 );
                }
                // TODO Capture here should do the PboRead and fill vPixelData...
                memcpy( &vPixelData[0], m_pNormalsMode->Capture(),
                        ImageWidth()*ImageHeight()*3 );
                return true;
            }
            return false;
        }


        /////////////////////////////////////////////////////////////////////////////////////////
        bool HasRGB()
        {
            return m_pRGBMode;
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        bool HasDepth()
        {
            return m_pDepthMode;
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        bool HasNormals()
        {
            return m_pNormalsMode;
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        unsigned ImageWidth()
        {
            return m_nSensorWidth;
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        unsigned ImageHeight()
        {
            return m_nSensorHeight;
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        void Begin()
        {
            // 0) push some attribs
            glPushAttrib( GL_COLOR_BUFFER_BIT );

            // 1) setup off-screen "camera" we will render to:
            glMatrixMode( GL_PROJECTION ); // change mode to save projection matrix
            glPushMatrix(); // push proj mat
            glLoadIdentity(); // now setup our own proj mat

            if( m_bOrthoCam ){
                glOrtho( m_fOrthoLeft, m_fOrthoRight,
                        m_fOrthoTop, // top and bottom are flipped
                        m_fOrthoBottom,
                        m_fNear, m_fFar );
            }
            else{
                //  double dfovx = 360.0*atan2( nSensorWidth/2.0, K(0,0) )/M_PI;
                double dfovy = 360.0*atan2( m_nSensorHeight/2.0, m_dK(1,1) )/M_PI;
                gluPerspective( dfovy, (float)m_nSensorWidth/(float)m_nSensorHeight, m_fNear, m_fFar );
            }

            // save OpenGL perspective projection parameters (from which we will pull focal lenght)
            glGetDoublev( GL_PROJECTION_MATRIX, m_dM.data() );

            glMatrixMode( GL_MODELVIEW ); // change back to model view
            glPushMatrix(); // push current MODELVIEW matrix (camera pose) since we're in modelview mode
            glLoadIdentity();
            Eigen::Vector3d c =  m_dPose.block<3,1>(0,3);
            Eigen::Vector3d f =  m_dPose.block<3,1>(0,0);
            Eigen::Vector3d t =  c+100*f; // way out in front
            Eigen::Vector3d u = -m_dPose.block<3,1>(0,2);

            gluLookAt( c[0], c[1], c[2], t[0], t[1], t[2], u[0], u[1], u[2] );

            // save camera pose
            glGetDoublev( GL_MODELVIEW_MATRIX, m_dT.data() );
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        void End()
        {
            // 3) go back to the main "camera" (i.e., the screen)
            glMatrixMode( GL_PROJECTION );
            glPopMatrix(); // pop projection marix
            glMatrixMode( GL_MODELVIEW );
            glPopMatrix(); // pop model view marix
            glPopAttrib();
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        Eigen::Matrix4d& GetModelViewMatrixRef()
        {
            return m_dT; // set in Begin
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        Eigen::Matrix4d& GetProjectionMatrixRef()
        {
            return m_dM; // set in Begin
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        void RenderToTexture()
        {
            Begin();
            if( m_pRGBMode ){
                m_pRGBMode->RenderToTexture();
            }
            if( m_pDepthMode ){
                m_pDepthMode->RenderToTexture();
            }
            if( m_pNormalsMode ){
                m_pNormalsMode->RenderToTexture();
            }

            End();
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        /// For registering our PreRender callback
        static void RenderCallBack( GLWindow*, void* pUserData )
        {
            GLSimCam* pThis = (GLSimCam*)pUserData;
            pThis->RenderToTexture();
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        /// Change the nuber of pixels in the sensor
        void SetSensorSize( unsigned int nWidth, unsigned int nHeight )
        {
            m_nSensorWidth = nWidth;
            m_nSensorHeight = nHeight;
            m_pFbo->SetTexWidth( nWidth );
            m_pFbo->SetTexHeight( nHeight );
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        // local little helper
        Eigen::Vector4d Vec4( double a, double b, double c, double d )
        {
            Eigen::Vector4d tmp;
            tmp << a, b, c, d;
            return tmp;
        }


        /////////////////////////////////////////////////////////////////////////////////////////
        void DrawCamera()
        {
            // OK
            Eigen::Matrix4d M = m_dM.inverse();
            Eigen::Matrix4d T = m_dT.inverse();

            // Map the normalized device coordinates back through the modelview
            // and projection matrices to find the viewing volume:
            Eigen::Vector4d lbn = T*M*Vec4( -1,-1,-1, 1 );  lbn/=lbn[3];
            Eigen::Vector4d rbn = T*M*Vec4(  1,-1,-1, 1 );  rbn/=rbn[3];
            Eigen::Vector4d ltn = T*M*Vec4( -1, 1,-1, 1 );  ltn/=ltn[3];
            Eigen::Vector4d rtn = T*M*Vec4(  1, 1,-1, 1 );  rtn/=rtn[3];

            Eigen::Vector4d lbf = T*M*Vec4( -1,-1, 1, 1 );  lbf/=lbf[3];
            Eigen::Vector4d rbf = T*M*Vec4(  1,-1, 1, 1 );  rbf/=rbf[3];
            Eigen::Vector4d ltf = T*M*Vec4( -1, 1, 1, 1 );  ltf/=ltf[3];
            Eigen::Vector4d rtf = T*M*Vec4(  1, 1, 1, 1 );  rtf/=rtf[3];

            /// Draw texture
            //glDisable( GL_BLEND );
            glColor4f( 1,1,1,1 );
            glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
            glEnable( GL_TEXTURE_RECTANGLE_ARB );
            // TODO: replace with first mode?
//            if( m_vModes.size() > 0 ){
//                glBindTexture( GL_TEXTURE_RECTANGLE_ARB, m_vModes.at(0).Texture() );
//            }
            if( m_pRGBMode ){
                glBindTexture( GL_TEXTURE_RECTANGLE_ARB, m_pRGBMode->Texture() );
            }

            glBegin( GL_QUADS );
            glNormal3f( -1,0,0 );
            glTexCoord2f(                 0.0,                 0.0  ); glVertex3dv( lbn.data() );
            glTexCoord2f( m_pFbo->m_nTexWidth,                 0.0  ); glVertex3dv( rbn.data() );
            glTexCoord2f( m_pFbo->m_nTexWidth, m_pFbo->m_nTexHeight ); glVertex3dv( rtn.data() );
            glTexCoord2f(                 0.0, m_pFbo->m_nTexHeight ); glVertex3dv( ltn.data() );
            glEnd();

            glBindTexture( GL_TEXTURE_RECTANGLE_ARB, 0 );
            glDisable(GL_TEXTURE_RECTANGLE_ARB);
            glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );


            /// draw frustum
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            glEnable( GL_DEPTH_TEST );
            glDisable( GL_LIGHTING );
            glDisable( GL_LIGHT0 );
            glDisable( GL_CULL_FACE );

            glColor4f( 0.9,0.9,1,0.1 );
            glBegin( GL_QUADS );

            // near
            glVertex3dv( lbn.data() );
            glVertex3dv( rbn.data() );
            glVertex3dv( rtn.data() );
            glVertex3dv( ltn.data() );


            // far
            glVertex3dv( rbf.data() );
            glVertex3dv( lbf.data() );
            glVertex3dv( ltf.data() );
            glVertex3dv( rtf.data() );

            // left
            glVertex3dv( lbf.data() );
            glVertex3dv( lbn.data() );
            glVertex3dv( ltn.data() );
            glVertex3dv( ltf.data() );

            // right
            glVertex3dv( rbn.data() );
            glVertex3dv( rbf.data() );
            glVertex3dv( rtf.data() );
            glVertex3dv( rtn.data() );

            // top
            glVertex3dv( ltn.data() );
            glVertex3dv( rtn.data() );
            glVertex3dv( rtf.data() );
            glVertex3dv( ltf.data() );

            // bottom
            glVertex3dv( lbn.data() );
            glVertex3dv( rbn.data() );
            glVertex3dv( rbf.data() );
            glVertex3dv( lbf.data() );

            glEnd();
            glDisable( GL_BLEND );
            glEnable( GL_DEPTH_TEST );
            glEnable( GL_CULL_FACE );


            // now draw coordinate frame and outline frustum
            double dScale = 1.0;
            Eigen::Vector3d c   =  m_dPose.block<3,1>(0,3);
            Eigen::Vector3d f   =  dScale*m_dPose.block<3,1>(0,0);
            Eigen::Vector3d r   =  dScale*m_dPose.block<3,1>(0,1);
            Eigen::Vector3d d   =  dScale*m_dPose.block<3,1>(0,2);

            glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
            glEnable( GL_BLEND );

            glBegin( GL_LINES );

            glColor4f( 1, 1, 1, 0.8 );

            // center to near clipping plane
            glVertex3dv( c.data() ); glVertex3dv( lbn.data() );
            glVertex3dv( c.data() ); glVertex3dv( rbn.data() );
            glVertex3dv( c.data() ); glVertex3dv( rtn.data() );
            glVertex3dv( c.data() ); glVertex3dv( ltn.data() );

            // near clipping plane to far clipping plane
            glVertex3dv( lbn.data() ); glVertex3dv( lbf.data() );
            glVertex3dv( rbn.data() ); glVertex3dv( rbf.data() );
            glVertex3dv( rtn.data() ); glVertex3dv( rtf.data() );
            glVertex3dv( ltn.data() ); glVertex3dv( ltf.data() );


            // x axis
            glColor4f( 1,0,0,1 );
            glVertex3d( c[0], c[1], c[2] );
            glVertex3d( c[0]+f[0], c[1]+f[1], c[2]+f[2] );

            // y axis
            glColor4f( 0,1,0,1 );
            glVertex3d( c[0], c[1], c[2] );
            glVertex3d( c[0]+r[0], c[1]+r[1], c[2]+r[2] );

            // z axis
            glColor4f( 0,0,1,1 );
            glVertex3d( c[0], c[1], c[2] );
            glVertex3d( c[0]+d[0], c[1]+d[1], c[2]+d[2] );

            // outline the image
            glColor4f( 1,1,1,1 );
            glVertex3dv( lbn.data() );
            glVertex3dv( rbn.data() );

            glVertex3dv( rbn.data() );
            glVertex3dv( rtn.data() );

            glVertex3dv( rtn.data() );
            glVertex3dv( ltn.data() );

            glVertex3dv( ltn.data() );
            glVertex3dv( lbn.data() );

            glEnd();
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        Eigen::Matrix3d GetKMatrix()
        {

            Eigen::Matrix4d M = m_dM.inverse();
            Eigen::Matrix4d T = m_dT.inverse();

            // Map the normalized device coordinates back through the modelview
            // and projection matrices to find the viewing volume:
            Eigen::Vector4d lbn = T*M*Vec4( -1,-1,-1, 1 );  lbn/=lbn[3];
            Eigen::Vector4d rbn = T*M*Vec4(  1,-1,-1, 1 );  rbn/=rbn[3];
            Eigen::Vector4d ltn = T*M*Vec4( -1, 1,-1, 1 );  ltn/=ltn[3];

            double dFocalLengthInMeters = M(0,0);
            // sensor_width/focal_length = near_width/near_dist, thus:
            double dNearPlaneWidth = (rbn-lbn).norm();
            double dNearPlaneHeight = (lbn-ltn).norm();
            double dSensorHeightInMeters = dFocalLengthInMeters*dNearPlaneHeight/m_fNear;
            double dSensorWidthInMeters = dFocalLengthInMeters*dNearPlaneWidth/m_fNear;
            // and in pixels:
            double fx = dFocalLengthInMeters * m_nSensorWidth / dSensorWidthInMeters;
            double fy = dFocalLengthInMeters * m_nSensorHeight / dSensorHeightInMeters;

            Eigen::Matrix3d K;
            K(0,0) = fx;
            K(0,1) = 0.0; // sx
            K(0,2) = m_nSensorWidth/2.0; // cx
            K(1,0) = 0.0;
            K(1,1) = fy; // fy
            K(1,2) = m_nSensorHeight/2.0; // cy
            K(2,0) = 0.0;
            K(2,1) = 0.0;
            K(2,2) = 1.0;

            return K;
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        void DrawRangeData()
        {
            std::vector<float> vRangeData;
            DepthTo3D( vRangeData );
            glPointSize( 2 );
            glEnable( GL_COLOR_MATERIAL );
            glColor3f( 1,0,1 );
            glBegin( GL_POINTS );
	    // vRangeData is a vector of 3d points
            for( size_t ii = 0; ii < vRangeData.size(); ii+=3 ){
                glVertex3fv( &vRangeData[ii] );
            }
            glEnd();
            glPointSize( 1 );

        }

        /////////////////////////////////////////////////////////////////////////////////////////
        /// use depth image and camera model to compute 3D range data
        // nb we flip the image here
        void DepthTo3D(  std::vector<float>& vRangeData  )
        {
            if( !HasDepth() ){
                return;
            }
            // transform depth to 3d
            Eigen::Matrix3d K = GetKMatrix();
            vRangeData.clear();
            float dx = (m_fOrthoRight-m_fOrthoLeft)/m_nSensorWidth;
            float dy = (m_fOrthoBottom-m_fOrthoTop)/m_nSensorHeight;
            float fx = K(0,0); // focal length in pixels
            float cx = K(0,2);
            float fy = K(1,1); // focal length in pixels
            float cy = K(1,2);
            //int n = 0;
            for( int jj = m_nSensorHeight-1; jj >=0  ; jj-- ){
                for( unsigned int ii = 0; ii <  m_nSensorWidth; ii++ ){
                    // NB to convet to xyz use fact that x/z = u/f
                    float x, y, z, u, v;
                    u  = ii-cx;
                    v  = (m_nSensorWidth-jj-1)-cy;
		    // Should work with the buffer already in the mode.
		    GLfloat* buff = (GLfloat*)m_pDepthMode->m_pBuffer;
                    z  = buff[m_nSensorWidth*jj + ii];
		    // z is normalized... undo
		    //float depthN = (depth - zNear)/(zFar - zNear); <-- from shader
		    // depthN * (zFar - zNear) = depth - zNear
		    // depth = (depthN * (zFar - zNear)) + zNear
		    z = (z * (m_fFar - m_fNear)) + m_fNear;

                    if( m_bOrthoCam ){
                        x  = dx*u;
                        y  = dy*v;
                    }
                    else{
                        x  = z*u/fx;
                        y  = z*v/fy;
                    }
                    Eigen::Vector4d p;
                    p << z,x,y,1;  // setup p using robot convention
//                    p = m_dPose*p; // and put p in the world frame

                    // don't add points at 0
                    if( z > 1e-6 ){
                        vRangeData.push_back( p[0] );
                        vRangeData.push_back( p[1] );
                        vRangeData.push_back( p[2] );
                    }
                }
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        /// should be private....
        FBO*                                        m_pFbo;
        GLSceneGraph*                               m_pSceneGraph;
    private:

        unsigned int                                m_nSensorWidth;
        unsigned int                                m_nSensorHeight;
        bool                                        m_bInitDone;
        GLuint                                      m_nDepthShaderProgram;
        GLuint                                      m_nNormalShaderProgram;
        float                                       m_fNear;
        float                                       m_fFar;
        bool                                        m_bOrthoCam; // if ortho, we will ignore K below
        float                                       m_fOrthoTop;
        float                                       m_fOrthoBottom;
        float                                       m_fOrthoLeft;
        float                                       m_fOrthoRight;
        Eigen::Matrix3d                             m_dK; // computer vision K matrix
        Eigen::Matrix4d                             m_dPose; // desired camera pose
        Eigen::Matrix<double,4,4,Eigen::ColMajor>   m_dM; // to save projection matrix
        Eigen::Matrix<double,4,4,Eigen::ColMajor>   m_dT; // to save modelview matrix
        SimCamMode*                                 m_pRGBMode;
        SimCamMode*                                 m_pDepthMode;
        SimCamMode*                                 m_pNormalsMode;
};

#endif
