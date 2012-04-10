#include <SimpleGui/Gui.h> // simple OpenGL scene graph using FLTK
#include <SimpleGui/GetPot> // for friendy command line parsing

#include <SimpleGui/Robot/urdf_parser.h>
#include <iostream>
#include <fstream>

#include <boost/shared_ptr.hpp>

using namespace RGUtils;

void printTree(boost::shared_ptr<const Link> link,int level = 0)
{
    level+=2;
    int count = 0;
    for (std::vector<boost::shared_ptr<Link> >::const_iterator child = link->child_links.begin(); child != link->child_links.end(); child++)
    {
        if (*child)
        {
            for(int j=0;j<level;j++) std::cout << "  "; //indent
            std::cout << "child(" << (count++)+1 << "):  " << (*child)->name  << std::endl;
            // first grandchild
            printTree(*child,level);
        }
        else
        {
            for(int j=0;j<level;j++) std::cout << " "; //indent
            std::cout << "root link: " << link->name << " has a null child!" << *child << std::endl;
        }
    }

}

//void recursiveMeshLoad( GLWindow* pWin, boost::shared_ptr<ModelInterface> robot, boost::shared_ptr<const Link> link ) {
void recursiveMeshLoad( GLWindow* pWin, boost::shared_ptr<ModelInterface> robot, std::string linkname ) {
    boost::shared_ptr<const Link> link = robot->getLink( linkname );

    if( link->visual != NULL ) {
        boost::shared_ptr<Geometry> geometry = link->visual->geometry;
        std::string sGeometryFile = geometry->filename;
        const struct aiScene* pScene = aiImportFile( sGeometryFile.c_str(), aiProcessPreset_TargetRealtime_MaxQuality );
        GLMesh* mesh = new GLMesh();
        mesh->Init( pScene );
        pWin->AddChildToRoot( mesh );
    }

    int nChildren = link->child_links.size();
    if( !nChildren )
        // there are no children
        return;

    // there must be children
    for( int i = 0; i < nChildren; i++  ) {

        boost::shared_ptr<const Link> childlink = link->child_links.at( i );
        std::string sChildLink = childlink->name;
        int z = 0;
        recursiveMeshLoad( pWin, robot, sChildLink );
    }
}

void loadGrid( GLWindow* pWin ) {
    GLGrid* grid = new GLGrid();
    grid->SetPerceptable( false );
    pWin->AddChildToRoot( grid );
}

int main(int argc, char** argv)
{
    std::string xml_string;
    std::fstream xml_file("katana.urdf", std::fstream::in);

    while ( xml_file.good() )
    {
        std::string line;
        std::getline( xml_file, line);
        xml_string += (line + "\n");
    }
    xml_file.close();

    boost::shared_ptr<ModelInterface> robot = parseURDF(xml_string);
    if (!robot){
        std::cerr << "ERROR: Model Parsing the xml failed" << std::endl;
        return -1;
    }
    std::cout << "robot name is: " << robot->getName() << std::endl;

    // get info from parser
    std::cout << "---------- Successfully Parsed XML ---------------" << std::endl;
    // get root link
    boost::shared_ptr<const Link> root_link=robot->getRoot();
    if (!root_link) return -1;

    std::cout << "root Link: " << root_link->name << " has " << root_link->child_links.size() << " child(ren)" << std::endl;

    // print entire tree
    // printTree(root_link);

    //deals with commandline
    GetPot cl( argc, argv );

    //init window
    GLWindow* pWin = new GLWindow(0, 0, 1024, 768, "URDF Demo");

    loadGrid( pWin );

    struct aiLogStream stream = aiGetPredefinedLogStream( aiDefaultLogStream_STDOUT, NULL );
    aiAttachLogStream( &stream );

    std::string sRootLinkName = robot->getRoot()->name;
    recursiveMeshLoad( pWin, robot, sRootLinkName );

    return (pWin->Run());
}
