#include <SimpleGui/Gui.h> // simple OpenGL scene graph using FLTK 
#include <SimpleGui/GetPot> // for friendy command line parsing

#include <IL/il.h> // include DevIL for image loading

using namespace Eigen;

// the global Assimp scene object
const aiScene* pScene = NULL;

// scale factor for the model to fit in the window
float scaleFactor;

// images / texture
// map image filenames to textureIds
// pointer to texture Array
std::map<std::string, GLuint> textureIdMap;	


// ----------------------------------------------------------------------------
// Load object from file

#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)

#include <CVarHelpers.h>
using namespace Eigen;

void get_bounding_box_for_node (const struct aiNode* nd, struct aiVector3D* min, 
														 struct aiVector3D* max)	
{
	struct aiMatrix4x4 prev;
	unsigned int n = 0, t;

	for (; n < nd->mNumMeshes; ++n) {
		const struct aiMesh* pMesh = pScene->mMeshes[nd->mMeshes[n]];
		for (t = 0; t < pMesh->mNumVertices; ++t) {

			struct aiVector3D tmp = pMesh->mVertices[t];

			min->x = aisgl_min(min->x,tmp.x);
			min->y = aisgl_min(min->y,tmp.y);
			min->z = aisgl_min(min->z,tmp.z);

			max->x = aisgl_max(max->x,tmp.x);
			max->y = aisgl_max(max->y,tmp.y);
			max->z = aisgl_max(max->z,tmp.z);
		}
	}

	for (n = 0; n < nd->mNumChildren; ++n) {
		get_bounding_box_for_node(nd->mChildren[n],min,max);
	}
}


void get_bounding_box (struct aiVector3D* min, struct aiVector3D* max)
{

	min->x = min->y = min->z =  1e10f;
	max->x = max->y = max->z = -1e10f;
	get_bounding_box_for_node(pScene->mRootNode,min,max);
}

//load 3ds object from file
bool Import3DFromFile( const std::string& pFile)
{

	//check if file exists
	std::ifstream fin(pFile.c_str());
	if(!fin.fail()) {
		fin.close();
	}
	else{
		//printf("Couldn't open file: %s\n", pFile.c_str());
		std::cout << "Couldn't open file " << pFile << std::endl;
		//printf("%s\n", importer.GetErrorString());
		return false;
	}

	
	pScene = aiImportFile( pFile.c_str(), aiProcessPreset_TargetRealtime_MaxQuality );
	//pScene = aiImportFile( pFile.c_str(), 0 );

	// If the import failed, report it
	if( !pScene)
	{
		std::cout << "Problem importing objects "<< aiGetErrorString() << std::endl;
		return false;
	}

	// Now we can access the file's contents.
	std::cout << "Import of scene " << pFile.c_str() << " succeeded.\n";

	struct aiVector3D scene_min, scene_max, scene_center;
	get_bounding_box(&scene_min, &scene_max);
	float tmp;
	tmp = scene_max.x-scene_min.x;
	tmp = scene_max.y - scene_min.y > tmp?scene_max.y - scene_min.y:tmp;
	tmp = scene_max.z - scene_min.z > tmp?scene_max.z - scene_min.z:tmp;
	scaleFactor = 1.f / tmp;

	//rotate the object
	aiMatrix3x3 rot, rotX;
	aiIdentityMatrix3( &rotX );
    aiMatrix3x3::Rotation( -AI_MATH_PI/2, aiVector3D( 1, 0, 0 ), rotX );
    aiIdentityMatrix3( &rot );
    aiMultiplyMatrix3( &rot, &rotX );
    
    // Process each vertex and alter it so that it is compatible with our world coordinate system
    unsigned int meshes = pScene->mNumMeshes;
    for( unsigned int meshidx = 0; meshidx < meshes; meshidx++ ) {
        aiMesh* mesh = pScene->mMeshes[meshidx];
        unsigned int vertices = mesh->mNumVertices;
        for( unsigned int vertexidx = 0; vertexidx < vertices; vertexidx++ ) {
            aiVector3D* vertex = &mesh->mVertices[vertexidx];
            aiTransformVecByMatrix3( vertex, &rot );
        }
    }
    
	// We're done. Everything will be cleaned up by the importer destructor
	return true;
}

// ----------------------------------------------------------------------------
// Load texture into scene

int LoadGLTextures(const aiScene* scene)
{
	ILboolean success;

	/* initialization of DevIL */
	ilInit(); 

	/* scan scene's materials for textures */
	for (unsigned int m=0; m < scene->mNumMaterials; ++m)
	{
		int texIndex = 0;
		aiString path;	// filename

		aiReturn texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
		while (texFound == AI_SUCCESS) {
			//fill map with textures, OpenGL image ids set to 0
			textureIdMap[path.data] = 0; 
			// more textures?
			texIndex++;
			texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
		}
	}

	int numTextures = textureIdMap.size();

	/* create and fill array with DevIL texture ids */
	ILuint* imageIds = new ILuint[numTextures];
	ilGenImages(numTextures, imageIds); 

	/* create and fill array with GL texture ids */
	GLuint* textureIds = new GLuint[numTextures];
	glGenTextures(numTextures, textureIds); /* Texture name generation */

	/* get iterator */
	std::map<std::string, GLuint>::iterator itr = textureIdMap.begin();
	int i=0;
	for (; itr != textureIdMap.end(); ++i, ++itr)
	{
		//save IL image ID
		std::string filename = (*itr).first;  // get filename
		(*itr).second = textureIds[i];	  // save texture id for filename in map

		ilBindImage(imageIds[i]); /* Binding of DevIL image name */
		ilEnable(IL_ORIGIN_SET);
		ilOriginFunc(IL_ORIGIN_LOWER_LEFT); 
		success = ilLoadImage((ILstring)filename.c_str());

		if (success) {
			/* Convert image to RGBA */
			ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE); 

			/* Create and load textures to OpenGL */
			glBindTexture(GL_TEXTURE_2D, textureIds[i]); 
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH),
				ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE,
				ilGetData()); 
		}
		else 
			printf("Couldn't load Image: %s\n", filename.c_str());
	}
	/* Because we have already copied image data into texture data
	we can release memory used by image. */
	ilDeleteImages(numTextures, imageIds); 

	//Cleanup
	delete [] imageIds;
	delete [] textureIds;

	//return success;
	return true;
}

int main( int argc, char** argv )
{
    GetPot cl( argc, argv );

    std::string sMesh = cl.follow( "bench.obj", 1, "-obj" );

    //std::string sMesh = cl.follow( "allrooms4.3DS", 1, "-obj" );

    // init window
    GLWindow* pWin = new GLWindow( 0, 0, 1024, 768, "Simple Gui Demo" );

    // load mesh
    //pScene = aiImportFile( sMesh.c_str(), aiProcessPreset_TargetRealtime_MaxQuality );    
	if (!Import3DFromFile(sMesh.c_str())) 
		return(0);

	LoadGLTextures(pScene);
	
	GLGrid grid;
    grid.SetPerceptable( false );
    GLMesh mesh;
    mesh.Init( pScene );
    CVarUtils::AttachCVar( "ObjectPose", &mesh.GetPoseRef(), "" );

    // register objects
    pWin->AddChildToRoot( &mesh );
    pWin->AddChildToRoot( &grid );

    return( pWin->Run() );
}
