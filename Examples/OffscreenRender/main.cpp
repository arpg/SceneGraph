#include <pangolin/pangolin.h>
#include <SceneGraph/SceneGraph.h>

#include <iostream>
#include <iomanip>

#include <boost/gil/gil_all.hpp>
#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL
#include <boost/gil/extension/io/png_io.hpp>

using namespace SceneGraph;
using namespace pangolin;
using namespace std;

void Usage() {
    cout << "Usage: OffscreenRender ModelFilename DestinationDirectory" << endl;
}
      
Eigen::Matrix<double,6,1> TrajectoryT_wx(double t)
{
    const double r = 7.5 + sin( 4*(M_PI+t/3) - M_PI/2.0)/2.0;
    Eigen::Matrix<double,6,1> ret;
    ret << r * cos(M_PI+t/3.0), r * sin(M_PI+t/3.0), -1.5,    M_PI/2, 0, t/3.0;
    return ret;
}

int main( int argc, char* argv[] )
{
    if(argc < 2) {
        Usage();
        return -1;
    }    

    bool save_files = (argc == 3);
    const std::string model_filename(argv[1]);    
    const std::string destination_directory = save_files ? argv[2] : "";

    const int w = 640;
    const int h = 480;
    
    // Create OpenGL window in single line thanks to GLUT
    pangolin::CreateGlutWindowAndBind("Main",640*2,480);
    GLSceneGraph::ApplyPreferredGlSettings();
    glewInit();

    // Scenegraph to hold GLObjects and relative transformations
    GLSceneGraph glGraph;
            
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
    
    // Coordinate axis to visualise trajectory
    GLAxis glCamAxis;
    glGraph.AddChild(&glCamAxis);    

    // Define Camera Render Object (for view / scene browsing)
    pangolin::OpenGlRenderState stacks3d(
        ProjectionMatrix(640,480,420,420,320,240,0.1,1000),
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
    ImageView viewImage(false,true);
    viewImage.SetBounds(0.0, 1.0, 1.0/2.0, 1.0, (double)w/h);

    // Add our views as children to the base container.
    pangolin::DisplayBase().AddDisplay(view3d);
    pangolin::DisplayBase().AddDisplay(viewImage);
    
    
    // Define Camera Render Object for generating synthetic video sequence
    pangolin::OpenGlRenderState stacks_synth(
        ProjectionMatrixRDF_TopLeft(640,480,420,420,320,240,0.1,1000),
        ModelViewLookAt(0,-2,-4, 0,1,0, AxisNegZ)
    );

    // Offscreen render buffer for synthetic video sequence
    pangolin::GlRenderBuffer synth_depth(w,h);
    pangolin::GlTexture synth_texture(w,h);
    pangolin::GlFramebuffer synth_framebuffer(synth_texture, synth_depth);

    // Time details
    const double LoopTime = 3*2*M_PI;
    const double frameTime = LoopTime / 200;
    const double lineTime = frameTime / h;
    int frame = 0;
    double time = 0; 
    
    boost::gil::rgba8_image_t img(w, h);
    unsigned char* img_data = boost::gil::interleaved_view_get_raw_data( boost::gil::view( img ) );
    
    // Default hooks for exiting (Esc) and fullscreen (tab).
    while( !pangolin::ShouldQuit() )
    {        
        glCamAxis.SetPose(GLCart2T(TrajectoryT_wx(time)));
        
        // Render synthetic rolling shutter scene
        synth_framebuffer.Bind();
        glViewport(0,0,w,h);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_SCISSOR_TEST);        
        for(int r=0; r<h; ++r) {            
            glScissor(0,r,w,1);
            Eigen::Matrix4d T_wv = GLCart2T(TrajectoryT_wx(time + r*lineTime));
            pangolin::OpenGlMatrix T_vw = OpenGlMatrix(T_wv).Inverse();
            stacks_synth.SetModelViewMatrix(T_vw);
            stacks_synth.Apply();
//            stacks3d.Apply();
            glMesh.DrawObjectAndChildren();
        }
        glDisable(GL_SCISSOR_TEST);        
        glFlush();        
        
        // read and save to file.
        glReadPixels(0,0,w,h, GL_RGBA, GL_UNSIGNED_BYTE, img_data );
        if(save_files) {
            std::ostringstream ss;
            ss << destination_directory << "/0_" << std::setw( 5 ) << std::setfill( '0' ) << frame << ".png";
            boost::gil::png_write_view(ss.str(), flipped_up_down_view( boost::gil::const_view(img)) );
        }
        synth_framebuffer.Unbind();
                
        time += frameTime;
        frame++;
        
        // Clear whole screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Display our saved image        
        viewImage.SetImage(img_data, w, h, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);

        // Swap frames and Process Events
        pangolin::FinishGlutFrame();

        // Pause for 1/60th of a second.
        usleep(1E6 / 60);
    }

    return 0;
}
