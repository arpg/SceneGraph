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
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        /// Store pose as a roboitcs frame pose
        void SetPose( const Eigen::Matrix4d& dPose )
        {
            m_dPose = dPose;
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
                double dFar = 500                //< Input: opengl far clipping plane
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

            // setup our frame buffer object for off screen rendering
            m_fbo.Init( nSensorWidth, nSensorHeight );

            // reserve memory buffers
            m_vImageData.resize( nSensorWidth*nSensorHeight );
            m_vDepthData.resize( nSensorWidth*nSensorHeight );

            m_bInitDone = true;
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        /// return pointer to internal RGB buffer
        unsigned char* RGBDataPtr()
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
            return m_fbo.m_vColorTextureIds[0]; // texture associated with GL_COLOR_ATTACHMENT0_EXT
        }

        GLuint DepthTexture()
        {
            return m_fbo.m_vColorTextureIds[1]; // texture associated with GL_COLOR_ATTACHMENT0_EXT
        }

        GLuint NormalTexture()
        {
            return m_fbo.m_vColorTextureIds[2]; // texture associated with GL_COLOR_ATTACHMENT0_EXT
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

//            double dfovx = 360.0*atan2( nSensorWidth/2.0, K(0,0) )/M_PI;
            double dfovy = 360.0*atan2( m_nSensorHeight/2.0, m_dK(1,1) )/M_PI;
            gluPerspective( dfovy, (float)m_nSensorWidth/(float)m_nSensorHeight, m_dNear, m_dFar );

            glMatrixMode( GL_MODELVIEW ); // change back to model view
            glPushMatrix(); // push current MODELVIEW matrix (camera pose) since we're in modelview mode
            glLoadIdentity();
            double x   =  m_dPose(0,3); // last col of T is cam pos
            double y   =  m_dPose(1,3);
            double z   =  m_dPose(2,3);
            double fx  =  m_dPose(0,0) + x; // first col of T is "forward" unit vector
            double fy  =  m_dPose(1,0) + y;
            double fz  =  m_dPose(2,0) + z;
            double upx = -m_dPose(0,2); // 34d col is "down" unit vector
            double upy = -m_dPose(1,2);
            double upz = -m_dPose(2,2);
            gluLookAt( x, y, z, fx, fy, fz, upx, upy, upz );

            glGetDoublev( GL_PROJECTION_MATRIX, m_dM.data() );
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
            ReadPixels( m_vImageData, m_nSensorWidth, m_nSensorHeight );

            // copy from surface normal buffer (also RGBA)
//            ReadPixels( m_vNormalData, m_nSensorWidth, m_nSensorHeight );
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        void RenderRGB()
        {
            m_fbo.Begin();
            glDrawBuffer( m_nRGBAAttachment ); // select fbo attachment...
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
            m_pSceneGraph->ApplyDfsVisitor( _ShaderVisitor );
            m_fbo.End();
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        void RenderNormals()
        {
            m_fbo.Begin();
            glDrawBuffer( m_nNormalAttachment ); // select fbo attachment...
            glUseProgram( m_nNormalShaderProgram );
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
            m_pSceneGraph->ApplyDfsVisitor( _ShaderVisitor );
            glUseProgram(0);
            m_fbo.End();
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        void RenderDepth()
        {
            m_fbo.Begin();
            glDrawBuffer( m_nDepthAttachment ); // select fbo attachment...
            glUseProgram( m_nDepthShaderProgram );
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
            m_pSceneGraph->ApplyDfsVisitor( _ShaderVisitor );
            glUseProgram(0);
            m_fbo.End();
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

    private:
        GLSceneGraph*                               m_pSceneGraph;
        FBO                                         m_fbo; // yes, one fbo per cam for now

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
        std::vector<unsigned char>                  m_vImageData;
        std::vector<float>                          m_vDepthData;
};

#endif
