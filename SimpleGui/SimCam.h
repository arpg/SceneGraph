#ifndef _SIMCAM_H_
#define _SIMCAM_H_

#include <SimpleGui/FBO.h>
#include <SimpleGui/GLSceneGraph.h>
#include <SimpleGui/GLSLHelpers.h>

#include <string>


class GLSimCam
{
    public:

        /////////////////////////////////////////////////////////////////////////////////////////
        GLSimCam()
        {
            m_bInitDone = false;
            m_pSceneGraph = NULL;
	    m_pFbo = FBO::Instance();
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        /// Store pose as a roboitcs frame pose
        void SetPose( const Eigen::Matrix4d& dPose )
        {
            m_dPose = dPose;
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
                double dNear = 1,                //< Input: opengl near clipping plane
                double dFar = 300                //< Input: opengl far clipping plane
                )
        {
            m_pSceneGraph = pSceneGraph;
            m_nSensorWidth = nSensorWidth;
            m_nSensorHeight = nSensorHeight;
            m_dNear = dNear;
            m_dFar = dFar;
            m_dK = dK;
            m_dPose = dPose;

            /*
            std::string sDepthVertShader = 
                "// see http://olivers.posterous.com/linear-depth-in-glsl-for-real\n"
                "varying float depth;\n"
                "void main(void)\n"
                "{\n"
                "    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;\n"
                "    depth = -(gl_ModelViewMatrix * gl_Vertex).z;\n"
                "}\n";


            std::string sDepthFragShader = 
                "// see http://olivers.posterous.com/linear-depth-in-glsl-for-real\n"
                "varying float depth;\n"
                "void main(void)\n"
                "{\n"
                "    float A = gl_ProjectionMatrix[2].z;\n"
                "    float B = gl_ProjectionMatrix[3].z;\n"
                "    float zNear = - B / (1.0 - A);\n"
                "    float zFar  =   B / (1.0 + A);\n"
                "    float d = 0.5*(-A*depth + B) / depth + 0.5;\n"
                "    //    gl_FragDepth  = d;\n"
                "    gl_FragColor = vec4( vec3(d), 1.0);\n"
                "}\n";

            InitShaders( sDepthVertShader, sDepthFragShader, m_nDepthShaderProgram );

            std::string sNormalVertShader = "";
            std::string sNormalFragShader = "";

            InitShaders( sNormalVertShader, sNormalFragShader, m_nNormalShaderProgram );

            */

            if ( LoadShaders( "Depth.vert", "Depth.frag", m_nDepthShaderProgram ) == false) {
                fprintf(stderr, "Failed to load the Depth shader.");
            }

            // Load shader
            if ( LoadShaders( "Normals.vert", "Normals.frag", m_nNormalShaderProgram ) == false) {
                fprintf(stderr, "Failed to load the Normal shader.");
            }

            // fixed order...
            m_nRGBAAttachment = GL_COLOR_ATTACHMENT0_EXT;
            m_nDepthAttachment = GL_COLOR_ATTACHMENT1_EXT;
            m_nNormalAttachment = GL_COLOR_ATTACHMENT2_EXT;

#if 0
            // Ok, now compute the corresponding GL_PROJECTION_MATRIX:
            double dfovx = 360.0*atan2( nSensorWidth/2.0, K(0,0) )/M_PI;
            double dfovy = 360.0*atan2( nSensorHeight/2.0, K(1,1) )/M_PI;

            /*
               from www.terathon.com/gdc07_lengyel.pdf:

           M = [ e           0              0            0
                 0         e/a              0            0
                 0           0   -(f+n)/(f-n)   -2fn/(f-n)
                 0           0             -1            0 ]

                 where
                 e = focal length
                 a = width/height of sensor (viewport)
                 n,f = near and far clipping planes

            */

            // why use fovx over fovy?
            double e = 1.0/(tan(dfovx/2.0)); // focal length
            double n = m_dNear;
            double f = m_dFar;
            double a = (double)nSensorWidth/(double)nSensorHeight;

            m_M = Eigeh::Matrix4d::Zero();
            m_M(0,0) = e;
            m_M(1,1) = e/a;
            m_M(2,2) = -(f+n)/(f-n);
            m_M(2,3) = -2.0*fn/(f-n);
            m_M(3,2) = -1;
#endif

	    // May have change this for multiple SimCams (since there is only one FBO)
            // setup our frame buffer object for off screen rendering
            m_pFbo->Init( nSensorWidth, nSensorHeight );

            // reserve memory buffers
            m_vImageData.resize( nSensorWidth*nSensorHeight );
            m_vDepthData.resize( nSensorWidth*nSensorHeight );

            m_bInitDone = true;
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        /// return pointer to internal RGB buffer
        //unsigned char* RGBDataPtr()
	float* RGBDataPtr()
        {
            return &m_vImageData[0];
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        /// Convert internal RGB data to greyscale
        char* GreyScaleDataPtr()
        {
            return NULL;
            // TODO
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        float* DepthDataPtr()
        {
            return &m_vDepthData[0];
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

        GLuint RGBTexture()
        {
            return m_pFbo->m_vColorTextureIds[0]; // texture associated with GL_COLOR_ATTACHMENT0_EXT
        }

        GLuint DepthTexture()
        {
            return m_pFbo->m_vColorTextureIds[1]; // texture associated with GL_COLOR_ATTACHMENT0_EXT
        }

        GLuint NormalTexture()
        {
            return m_pFbo->m_vColorTextureIds[2]; // texture associated with GL_COLOR_ATTACHMENT0_EXT
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

            //  double dfovx = 360.0*atan2( nSensorWidth/2.0, K(0,0) )/M_PI;
            double dfovy = 360.0*atan2( m_nSensorHeight/2.0, m_dK(1,1) )/M_PI;
            gluPerspective( dfovy, (float)m_nSensorWidth/(float)m_nSensorHeight, m_dNear, m_dFar );

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
        /// 
        void Render()
        {
            Begin();
            RenderRGB();
            RenderDepth();
            RenderNormals();
            End();

            // copy from depth buffer
            ReadDepthPixels( m_vDepthData, m_nSensorWidth, m_nSensorHeight );

            // copy from RGBA buffer
	    // !!! Using ReadPixels with a vector of unsigned ints causes crashes for me,
	    // I think it has something to do with resizing the vector. - James Marshall
            //ReadPixels( m_vImageData, m_nSensorWidth, m_nSensorHeight );
	    ReadDepthPixels( m_vImageData, m_nSensorWidth, m_nSensorHeight);
	    
            // copy from surface normal buffer (also RGBA)
//            ReadPixels( m_vNormalData, m_nSensorWidth, m_nSensorHeight );
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        void RenderRGB()
        {
            m_pFbo->Begin();
            glDrawBuffer( m_nRGBAAttachment ); // select fbo attachment...
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
            m_pSceneGraph->ApplyDfsVisitor( _ShaderVisitor );
            m_pFbo->End();
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        void RenderNormals()
        {
            m_pFbo->Begin();
            glDrawBuffer( m_nNormalAttachment ); // select fbo attachment...
            glUseProgram( m_nNormalShaderProgram );
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
            m_pSceneGraph->ApplyDfsVisitor( _ShaderVisitor );
            glUseProgram(0);
            m_pFbo->End();
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        void RenderDepth()
        {
            m_pFbo->Begin();
            glDrawBuffer( m_nDepthAttachment ); // select fbo attachment...
            glUseProgram( m_nDepthShaderProgram );
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
            m_pSceneGraph->ApplyDfsVisitor( _ShaderVisitor );
            glUseProgram(0);
            m_pFbo->End();
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        /// For registering our PreRender callback
        static void RenderCallBack( GLWindow*, void* pUserData )
        {
            GLSimCam* pThis = (GLSimCam*)pUserData;
            pThis->Render();
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        /// used to traverse scene graph
        static void _ShaderVisitor( GLObject* pObj )
        {
            if( pObj->IsPerceptable() ) {
                pObj->draw();
            }
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

            Eigen::Vector4d lbn = T*M*Vec4( -1,-1,-1, 1 );  lbn/=lbn[3];
            Eigen::Vector4d rbn = T*M*Vec4(  1,-1,-1, 1 );  rbn/=rbn[3];
            Eigen::Vector4d ltn = T*M*Vec4( -1, 1,-1, 1 );  ltn/=ltn[3];
            Eigen::Vector4d rtn = T*M*Vec4(  1, 1,-1, 1 );  rtn/=rtn[3];

            Eigen::Vector4d lbf = T*M*Vec4( -1,-1, 1, 1 );  lbf/=lbf[3];
            Eigen::Vector4d rbf = T*M*Vec4(  1,-1, 1, 1 );  rbf/=rbf[3];
            Eigen::Vector4d ltf = T*M*Vec4( -1, 1, 1, 1 );  ltf/=ltf[3];
            Eigen::Vector4d rtf = T*M*Vec4(  1, 1, 1, 1 );  rtf/=rtf[3];

            /// Draw texture
            glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
            glEnable( GL_TEXTURE_RECTANGLE_ARB );
            glBindTexture( GL_TEXTURE_RECTANGLE_ARB, RGBTexture() );

            glBegin( GL_QUADS );
            glNormal3f( -1,0,0 );
            glTexCoord2f(               0.0,               0.0  ); glVertex3dv( lbn.data() );
            glTexCoord2f( m_pFbo->m_nTexWidth,               0.0  ); glVertex3dv( rbn.data() );
            glTexCoord2f( m_pFbo->m_nTexWidth, m_pFbo->m_nTexHeight ); glVertex3dv( rtn.data() );
            glTexCoord2f(               0.0, m_pFbo->m_nTexHeight ); glVertex3dv( ltn.data() );
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

            // center to far clipping plane
            glVertex3dv( c.data() ); glVertex3dv( lbf.data() );
            glVertex3dv( c.data() ); glVertex3dv( rbf.data() );
            glVertex3dv( c.data() ); glVertex3dv( rtf.data() );
            glVertex3dv( c.data() ); glVertex3dv( ltf.data() );

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

           // z axis
//            glColor4f( 1,0.8,0.8,0.1 );
//            glVertex3dv( c.data() );
//            glVertex3dv( lbn.data() );

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


    private:
        GLSceneGraph*                               m_pSceneGraph;
        FBO*                                        m_pFbo;
        bool                                        m_bInitDone;
        GLuint                                      m_nDepthShaderProgram;
        GLuint                                      m_nNormalShaderProgram;
        int                                         m_nDepthAttachment;
        int                                         m_nNormalAttachment;
        int                                         m_nRGBAAttachment;
        unsigned int                                m_nSensorWidth;
        unsigned int                                m_nSensorHeight;
        double                                      m_dNear;
        double                                      m_dFar;
        Eigen::Matrix3d                             m_dK; // computer vision K matrix 
        Eigen::Matrix4d                             m_dPose; // desired camera pose
        Eigen::Matrix<double,4,4,Eigen::ColMajor>   m_dM; // to save projection matrix
        Eigen::Matrix<double,4,4,Eigen::ColMajor>   m_dT; // to save modelview matrix
        //std::vector<unsigned char>                  m_vImageData;
	std::vector<float>                          m_vImageData;
        std::vector<float>                          m_vDepthData;
};

#endif
