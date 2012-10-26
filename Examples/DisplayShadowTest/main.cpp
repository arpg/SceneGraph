#include <iostream>
#include <boost/bind.hpp>
#include <Eigen/Eigen>

#include <pangolin/pangolin.h>
#include <pangolin/gl.h>
#include <SceneGraph/SceneGraph.h>

using namespace std;

int main( int /*argc*/, char** /*argv[]*/ )
{
    // Create OpenGL window in single line thanks to GLUT
    pangolin::CreateGlutWindowAndBind("Main",640,480);
    SceneGraph::GLSceneGraph::ApplyPreferredGlSettings();
    glewInit();
    glClearColor(0,0,0,1);

    // Offscreen framebuffer
    const int sbw = 2048;
    const int sbh = 2048;
    pangolin::GlTexture fb_img(sbw,sbh);
    pangolin::GlRenderBuffer fb_depth(sbw,sbh,GL_DEPTH_COMPONENT32);
    pangolin::GlFramebuffer framebuffer(fb_img,fb_depth);

    CheckForGLErrors();
    pangolin::GlTexture depth_tex(sbw,sbh,GL_DEPTH_COMPONENT32,true,1,GL_DEPTH_COMPONENT,GL_UNSIGNED_BYTE);
    CheckForGLErrors();
    depth_tex.Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    CheckForGLErrors();
    GLfloat color[4] = {1,1,1,1};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
    CheckForGLErrors();


//    GLuint depth_tex;
//    glGenTextures(1,&depth_tex);
//    glBindTexture(GL_TEXTURE_2D, depth_tex);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, sbw,sbh, 1, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);

    // Scenegraph to hold GLObjects and relative transformations
    SceneGraph::GLSceneGraph glGraph;
    glGraph.AddLight(Eigen::Vector3d(10,10,-20));
    glGraph.ShowLights();

    // Define grid object
    SceneGraph::GLGrid glGrid(50,2.0, true);

    // Define axis object, and set its pose
    SceneGraph::GLAxis glAxis;
    glAxis.SetPose(-1,-2,-0.1, 0, 0, M_PI/4);
    glAxis.SetScale(0.25);

    // Define 3D spiral using a line strip object
    SceneGraph::GLLineStrip glLineStrip;
    for(double t=0; t < 10*M_PI; t+= M_PI/10) {
        glLineStrip.SetPoint(cos(t)+2, sin(t)+2, -0.2*t);
    }

    // Define a mesh object and try to load model
    SceneGraph::GLMesh glMesh;
    try {
        glMesh.Init("./model.blend");
//        glMesh.SetPosition(0,0,-0.15);
        glMesh.SetPosition(0,0,-1);
        glMesh.SetScale(4.0f);
        glGraph.AddChild(&glMesh);
    }catch(exception e) {
        cerr << "Cannot load mesh. Check file exists" << endl;
    }

    SceneGraph::GLCube glCube;
//    glCube.ClearTexture();
    glCube.SetPose(1.5,1.5,-sqrt(3), M_PI/4, M_PI/4, M_PI/4);

    // Add objects to scenegraph
    glGraph.AddChild(&glGrid);
    glGraph.AddChild(&glLineStrip);
    glGraph.AddChild(&glAxis);
    glGraph.AddChild(&glCube);

    // Define Camera Render Object (for view / scene browsing)
    pangolin::OpenGlRenderState stacks3d(
        pangolin::ProjectionMatrix(640,480,420,420,320,240,0.1,1000),
        pangolin::ModelViewLookAt(0,-2,-4, 0,1,0, pangolin::AxisNegZ)
    );

    // Pangolin abstracts the OpenGL viewport as a View.
    // Here we get a reference to the default 'base' view.
    pangolin::View& container = pangolin::DisplayBase();

    // We define a new view which will reside within the container.
    pangolin::View view3d;

    // We set the views location on screen and add a handler which will
    // let user input update the model_view matrix (stacks3d) and feed through
    // to our scenegraph
    view3d.SetBounds(0.0, 1.0, 0.0, 1.0, 640.0f/480.0f)
          .SetHandler(new SceneGraph::HandlerSceneGraph(glGraph,stacks3d,pangolin::AxisNegZ))
          ;//.SetDrawFunction(SceneGraph::ActivateDrawFunctor(glGraph, stacks3d));

    // Add our view as children to the base container.
    container.AddDisplay(view3d);

    SceneGraph::GLLight& light = glGraph.GetLight(0);

    // Default hooks for exiting (Esc) and fullscreen (tab).
    for(int frame=0; !pangolin::ShouldQuit(); ++frame )
    {
        // Clear whole screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        light.SetPose(100*cos(frame/100.0), 100*sin(frame/100.0), -100, 0,0,0);
        // Point light at scene
        const Eigen::Vector3d lpos = light.GetPose().head<3>();
        pangolin::OpenGlRenderState stacks_light(
            pangolin::ProjectionMatrix(sbw,sbh,50000,50000,sbw/2.0f,sbh/2.0f, 90,400),
            pangolin::ModelViewLookAt(lpos(0), lpos(1), lpos(2), 0,0,0, pangolin::AxisNegZ)
        );


        // Put depthmap from light into depth_tex
        {
            glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT);
            framebuffer.Bind();
            glShadeModel(GL_FLAT);
            glDisable(GL_LIGHTING);
            glDisable(GL_COLOR_MATERIAL);
            glDisable(GL_NORMALIZE);
            glColorMask(0, 0, 0, 0);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(2.0, 4.0);

            glViewport(0,0,fb_img.width,fb_img.height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            stacks_light.Apply();
            glGraph.DrawObjectAndChildren();

            depth_tex.Bind();
    //        glBindTexture(GL_TEXTURE_2D, depth_tex);
            glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 0, 0, fb_img.width, fb_img.height, 0);

            framebuffer.Unbind();
            glPopAttrib();
            CheckForGLErrors();
        }

        view3d.Activate(stacks3d);
        // Draw Scene first time
        {
            view3d.Activate(stacks3d);
            light.ApplyAsGlLight(GL_LIGHT0);

            GLfloat ambientLight [] = {0.2, 0.2, 0.2, 1.0};
            GLfloat diffuseLight [] = {0.4, 0.4, 0.4, 1.0};
            glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
            glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);

            glGraph.DrawObjectAndChildren();
        }

        // Modulate colour using depthmap
        {
//            glClear(GL_DEPTH_BUFFER_BIT);

            glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT);

            // Setup modelview and projection matrices
            stacks3d.Apply();

            glActiveTextureARB(GL_TEXTURE1_ARB);

            // Setup Texture matrix
            glMatrixMode(GL_TEXTURE);
            glLoadIdentity();
            glTranslatef(0.5f, 0.5f, 0.5f);
            glScalef(0.5f, 0.5f, 0.5f);
            stacks_light.GetProjectionMatrix().Multiply();
            stacks_light.GetModelViewMatrix().Multiply();
            glMatrixMode(GL_MODELVIEW);

            // Setup Automatic texture generation
            GLfloat sPlane[4] = {1.0f, 0.0f, 0.0f, 0.0f};
            GLfloat tPlane[4] = {0.0f, 1.0f, 0.0f, 0.0f};
            GLfloat rPlane[4] = {0.0f, 0.0f, 1.0f, 0.0f};
            GLfloat qPlane[4] = {0.0f, 0.0f, 0.0f, 1.0f};
            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);
            glEnable(GL_TEXTURE_GEN_R);
            glEnable(GL_TEXTURE_GEN_Q);
            glTexGenfv(GL_S, GL_EYE_PLANE, sPlane);
            glTexGenfv(GL_T, GL_EYE_PLANE, tPlane);
            glTexGenfv(GL_R, GL_EYE_PLANE, rPlane);
            glTexGenfv(GL_Q, GL_EYE_PLANE, qPlane);
            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
            glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
            glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);
            depth_tex.Bind();
//            glBindTexture(GL_TEXTURE_2D, depth_tex);

            GLfloat lowAmbient[4] = {0.01f, 0.01f, 0.01f, 1.0f};
            GLfloat lowDiffuse[4] = {0.01f, 0.01f, 0.01f, 1.0f};
            glLightfv(GL_LIGHT0, GL_AMBIENT, lowAmbient);
            glLightfv(GL_LIGHT0, GL_DIFFUSE, lowDiffuse);

            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//            glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE );
            glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_GEQUAL );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FAIL_VALUE_ARB, 0.5f);

            glAlphaFunc(GL_GREATER, 0.9f);
            glEnable(GL_ALPHA_TEST);


            glEnable(GL_TEXTURE_2D);
            glActiveTextureARB(GL_TEXTURE0_ARB);

            glGraph.DrawObjectAndChildren();

            glActiveTextureARB(GL_TEXTURE1_ARB);
            glDisable(GL_TEXTURE_2D);
            glActiveTextureARB(GL_TEXTURE0_ARB);

            glPopAttrib();
            CheckForGLErrors();
        }

//        // Put part of scene into darkness by modulating new texture on top
//        GLfloat ambientLight[4] = {0.2, 0.2, 0.2, 1.0};
//        GLfloat diffuseLight[4] = {0.7, 0.7, 0.7, 1.0};

//        glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
//        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);

        // Swap frames and Process Events
        pangolin::FinishGlutFrame();

        // Pause for 1/60th of a second.
        usleep(1E6 / 60);
    }

    return 0;
}
