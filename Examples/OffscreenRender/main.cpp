#include <pangolin/pangolin.h>
#include <SceneGraph/SceneGraph.h>

#include <iostream>
#include <iomanip>

using namespace SceneGraph;
using namespace pangolin;
using namespace std;

void Usage() {
    cout << "Usage: OffscreenRender ModelFilename DestinationDirectory" << endl;
}
      
Eigen::Matrix<double,6,1> TrajectoryT_wx(double t)
{
    const double s = t/3;
    const double r = 7.5 + sin( 4*(M_PI+s) - M_PI/2.0)/2.0;
    Eigen::Matrix<double,6,1> ret;
    
    ret << r * cos(M_PI+s), r * sin(M_PI+s), -1.5 + 0.05*sin(10*t),
            M_PI/2.0, 0,
            M_PI/2.0- atan2(
                (2.0*sin(4.0*(s+M_PI))*sin(s))/3.0+((7.5-cos(4.0*(s+M_PI))/2.0)*cos(s))/3.0,
                ((7.5-cos(4.0*(s+M_PI))/2.0)*sin(s))/3.0-(2.0*sin(4.0*(s+M_PI))*cos(s))/3.0
            );
    return ret;
}

void SavePPM( const std::string& prefix, unsigned char* img_data, int w, int h, int channels, double time )
{
    assert(channels == 1 || channels == 3);
    std::ofstream bFile( (prefix+".pgm").c_str(), std::ios::out | std::ios::binary );
    bFile << (channels == 1 ? "P5" : "P6") << std::endl;
    bFile << w << " " << h << '\n';
    bFile << "255" << '\n';
    bFile.write( (char *)img_data, w*h*channels);
    bFile.close();    
    
    std::ofstream txtFile( (prefix+".txt").c_str(), std::ios::out );
    txtFile << "%YAML:1.0" << std::endl;
    txtFile << "SystemTime: \"" << std::setprecision(20) << time << "\"" << std::endl;
    txtFile.close();
}

int main( int argc, char* argv[] )
{
    if(argc < 2) {
        Usage();
        return -1;
    }    

    const bool save_files = (argc == 3);
    const std::string model_filename(argv[1]);    
    const std::string destination_directory = save_files ? argv[2] : "";

    // Camera parameters
    const int w = 512;
    const int h = 384;
    const double fu = 300;
    const double fv = 300;
    const double u0 = w/2;
    const double v0 = h/2;
    
    // Create OpenGL window in single line thanks to GLUT
    pangolin::CreateGlutWindowAndBind("Main",640*2,480);
    GLSceneGraph::ApplyPreferredGlSettings();
    glewInit();

    // Scenegraph to hold GLObjects and relative transformations
    GLSceneGraph glGraph;
            
#ifdef HAVE_ASSIMP
    // Define a mesh object and try to load model
    SceneGraph::GLMesh glMesh;
    try {
        glMesh.Init(model_filename);
        glGraph.AddChild(&glMesh);
    }catch(exception e) {
        cerr << "Cannot load mesh. Check file exists" << endl;
        cerr << e.what() << endl;
        return -1;
    }
#endif // HAVE_ASSIMP
    
    // Coordinate axis to visualise trajectory
    GLAxis glCamAxis;
    glGraph.AddChild(&glCamAxis);    

    // Define Camera Render Object (for view / scene browsing)
    pangolin::OpenGlRenderState stacks3d(
        ProjectionMatrix(w,h,fu,fv,u0,v0,0.1,1000),
        ModelViewLookAt(0,-10,-30, 0,0,-1.5, AxisNegZ)
    );
    
    // We define a new view which will reside within the container.
    pangolin::View view3d;

    // We set the views location on screen and add a handler which will
    // let user input update the model_view matrix (stacks3d) and feed through
    // to our scenegraph
    view3d.SetBounds(0.0, 1.0, 0.0, 1.0/2.0, 640.0f/480.0f)
          .SetHandler(new HandlerSceneGraph(glGraph,stacks3d,AxisNegZ))
          .SetDrawFunction(ActivateDrawFunctor(glGraph, stacks3d));

    // We define a special type of view which will accept image data
    // to display and set its bounds on screen.
    pangolin::View viewImage;
    viewImage.SetBounds(0.0, 1.0, 1.0/2.0, 1.0, (double)w/h);

    // Add our views as children to the base container.
    pangolin::DisplayBase().AddDisplay(view3d);
    pangolin::DisplayBase().AddDisplay(viewImage);
    
    
    // Define Camera Render Object for generating synthetic video sequence
    pangolin::OpenGlRenderState stacks_synth(
        ProjectionMatrixRDF_BottomLeft(w,h,fu,fv,u0,v0,0.1,1000),
        ModelViewLookAt(0,-2,-4, 0,1,0, AxisNegZ)
    );

    // Offscreen render buffer for synthetic video sequence
    pangolin::GlRenderBuffer synth_depth(w,h);
    pangolin::GlTexture synth_texture(w,h,GL_RGBA);
    pangolin::GlFramebuffer synth_framebuffer(synth_texture, synth_depth);
    
    // Time details
    const double LoopDuration = 3*2*M_PI;
    const double frameDuration = LoopDuration / 200;
    const double lineDuration = frameDuration / h;
    int frame = 0;
    
    // frameStartTime is middle of first row in image.
    double frameStartTime = 0; 
    
    unsigned char* img_data = new unsigned char[w*h];
    
    // Default hooks for exiting (Esc) and fullscreen (tab).
    while( !pangolin::ShouldQuit() )
    {        
        glCamAxis.SetPose(GLCart2T(TrajectoryT_wx(frameStartTime)));
        
        // Render synthetic rolling shutter scene
        synth_framebuffer.Bind();
        glViewport(0,0,w,h);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glEnable(GL_SCISSOR_TEST); 
        for(int r=0; r<h; ++r) {            
            glScissor(0,r,w,1);
            Eigen::Matrix4d T_wv = GLCart2T(TrajectoryT_wx(frameStartTime + r*lineDuration));
            pangolin::OpenGlMatrix T_vw = OpenGlMatrix(T_wv).Inverse();
            stacks_synth.SetModelViewMatrix(T_vw);
            stacks_synth.Apply();
            glGraph.DrawObjectAndChildren(eRenderPerceptable);
        }
        glDisable(GL_SCISSOR_TEST);        
        synth_framebuffer.Unbind();

        if(save_files) {
            const double frameEndTime = frameStartTime + frameDuration;
            synth_texture.Download(img_data, GL_RED, GL_UNSIGNED_BYTE);
            std::ostringstream ss;
            ss << destination_directory << "/0_" << std::setw( 5 ) << std::setfill( '0' ) << frame;
            SavePPM(ss.str(), img_data, w, h, 1, frameEndTime );   
        }
                
        frameStartTime += frameDuration;
        frame++;
        
        // Clear whole screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Display our saved image        
        viewImage.Activate();
        synth_texture.RenderToViewportFlipY();

        // Swap frames and Process Events
        pangolin::FinishGlutFrame();

        // Pause for 1/60th of a second.
        usleep(1E6 / 60);
    }
    
    delete[] img_data;

    return 0;
}
