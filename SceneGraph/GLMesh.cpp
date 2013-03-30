#include "GLMesh.h"

#include "GLHelpersLoadTextures.h"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>

namespace SceneGraph
{

////////////////////////////////////////////////////////////////////////////
// Simple inline utilities for this class
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
inline void color4_to_float4( const aiColor4D *c, float f[4] )
{
    f[0] = c->r;
    f[1] = c->g;
    f[2] = c->b;
    f[3] = c->a;
}

////////////////////////////////////////////////////////////////////////////
inline void set_float4(float f[4], float a, float b, float c, float d)
{
    f[0] = a;
    f[1] = b;
    f[2] = c;
    f[3] = d;
}

////////////////////////////////////////////////////////////////////////////
inline GLenum GLWrapFromAiMapMode(aiTextureMapMode mode)
{
    switch(mode) {
        case aiTextureMapMode_Wrap:
            return GL_REPEAT;
        case aiTextureMapMode_Clamp:
            return GL_CLAMP;
        default:
            std::cerr << "Unsupported aiTextureMapMode used" << std::endl;
            return GL_CLAMP;
    }
}

////////////////////////////////////////////////////////////////////////////
// GLMesh Class implementation
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
GLMesh::GLMesh()
    : GLObject("Mesh"), m_fAlpha(1),
      m_iMeshID(-1), m_bShowMeshNormals(false)
{
}

////////////////////////////////////////////////////////////////////////////
GLMesh::GLMesh(const std::string& sMeshFile)
    : GLObject("Mesh"), m_fAlpha(1),
      m_iMeshID(-1), m_bShowMeshNormals(false)
{
    Init(sMeshFile);
}

////////////////////////////////////////////////////////////////////////////
GLMesh::~GLMesh()
{
    // Delete textures
    for(std::map<std::string,GLuint>::iterator i = m_mapPathToGLTex.begin(); i!= m_mapPathToGLTex.end(); ++i)
    {
        glDeleteTextures(1, &i->second);
    }

    // Free scene
    aiReleaseImport(m_pScene);
}

////////////////////////////////////////////////////////////////////////////
void GLMesh::Init( const std::string& sMeshFile, bool bFlipUVs )
{
    m_pScene = aiImportFile( sMeshFile.c_str(), aiProcess_Triangulate
                                            | aiProcess_GenSmoothNormals
                                            | aiProcess_JoinIdenticalVertices
//													| aiProcess_TransformUVCoords
                                            | ( bFlipUVs == true ? aiProcess_FlipUVs : 0 )
//													| aiProcess_FlipWindingOrder
                                            | aiProcess_OptimizeMeshes
                                            | aiProcess_FindInvalidData
//													| aiProcess_SortByPType
//													| aiProcess_GenUVCoords
                                            | aiProcess_FixInfacingNormals
                            );
    if( m_pScene == NULL ){
        throw GLMeshException("Unable to load mesh.");
    }else{
        Init(m_pScene);
    }
}

////////////////////////////////////////////////////////////////////////////
void GLMesh::Init( const struct aiScene* pScene )
{
    m_pScene = pScene;

    if(m_pScene != NULL ) {
        LoadMeshTextures();
        ComputeDimensions();
    }
}

////////////////////////////////////////////////////////////////////////////
void GLMesh::ShowNormals(bool showNormals )
{
    m_bShowMeshNormals = showNormals;
}

////////////////////////////////////////////////////////////////////////////
void  GLMesh::DrawCanonicalObject()
{
    if( m_pScene ){
        if( m_nDisplayList == -1 ){
            m_nDisplayList = glGenLists(1);
            glNewList( m_nDisplayList, GL_COMPILE_AND_EXECUTE );
            DrawCanonicalObject();
            glEndList();
        }else{
            glPushAttrib(GL_ENABLE_BIT);

            // Enable Multisample if it is available.
            glEnable(GL_MULTISAMPLE);                        
            glEnable(GL_BLEND);
            glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            
            glColor4f(1,1,1,m_fAlpha);
            RecursiveRender( m_pScene, m_pScene->mRootNode );

            glDisable(GL_BLEND);

            glPopAttrib();
        }
    }
}
\
void GLMesh::ComputeNodeBounds( const struct aiScene *pAIScene, const struct aiNode *pAINode, AxisAlignedBoundingBox& aabb, aiMatrix4x4 dParentTransform )
{
    aiMesh *pAIMesh;

    for ( unsigned int x = 0; x < pAINode->mNumMeshes; x++ )
    {
        pAIMesh = pAIScene->mMeshes[pAINode->mMeshes[x]];



        for( unsigned int y = 0; y < pAIMesh->mNumVertices; y++ ){

            aiVector3D pAIVector = dParentTransform * pAIMesh->mVertices[y];
            //aiVector3D pAIVector = pAINode->mTransformation * pAIMesh->mVertices[y];

                const Eigen::Vector3d p = Eigen::Vector3d(pAIVector.x,pAIVector.y,pAIVector.z) ;
                m_aabb.Insert(p);

        }
    }

    for ( unsigned int x = 0; x < pAINode->mNumChildren; x++ ){
        ComputeNodeBounds( pAIScene, pAINode->mChildren[x], aabb,dParentTransform*pAINode->mChildren[x]->mTransformation);
    }
}

///////////////////////////
void GLMesh::SetScale(Eigen::Vector3d s)
{
    GLObject::SetScale(s);
    ComputeDimensions();
}

///////////////////////////
void GLMesh::SetScale(double s)
{
    GLObject::SetScale(s);
    ComputeDimensions();
}


///////////////////////////
void GLMesh::ComputeDimensions()
{
    m_aabb.Clear();

    ComputeNodeBounds(this->GetScene(),this->GetScene()->mRootNode,m_aabb,this->GetScene()->mRootNode->mTransformation);
    m_aabb.Min().array() *= GLObject::GetScale().array();
    m_aabb.Max().array() *= GLObject::GetScale().array();
}

// Getters and setters
const struct aiScene *GLMesh::GetScene( void ) {
    return m_pScene;
}

Eigen::Vector3d GLMesh::GetDimensions() {
    return m_aabb.Size();
}

////////////////////////////////////////////////////////////////////////////
void GLMesh::LoadMeshTextures()
{
#ifdef HAVE_DEVIL
    // Ensure DevIL library is initialised
    static bool firsttime = true;
    if(firsttime) {
        ilInit();
        iluInit();
        firsttime = false;
    }
#endif // HAVE_DEVIL

    // For each material, find associated textures
    for (unsigned int m=0; m < m_pScene->mNumMaterials; ++m) {
        LoadMaterialTextures(m_pScene->mMaterials[m]);
    }
}

////////////////////////////////////////////////////////////////////////////
void GLMesh::LoadMaterialTextures(aiMaterial* pMaterial)
{
    // Try to load textures of any type
    for(aiTextureType tt = aiTextureType_NONE; tt <= aiTextureType_UNKNOWN; tt = (aiTextureType)((int)tt +1) ) {
        const unsigned int numTex = pMaterial->GetTextureCount(tt);

        for(unsigned int dt=0; dt < numTex; ++dt ) {
            aiString path;

            aiTextureMapping* mapping = 0;
            unsigned int* uvindex = 0;
            float* blend = 0;
            aiTextureOp* op = 0;
            aiTextureMapMode* mapmode = 0;

            aiReturn texFound = pMaterial->GetTexture(tt, dt, &path, mapping, uvindex, blend, op, mapmode);

            // Attempt to load reference to texture data as OpenGL
            // texture, with appropriate properties set.
            if( texFound == AI_SUCCESS ) {
                GLuint glTex = LoadGLTextureResource(path);

                if(glTex > 0 ) {
                    m_mapPathToGLTex[path.data] = glTex;
                    glBindTexture(GL_TEXTURE_2D, glTex);

                    // Use bilinear interpolation for textures
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

                    if(mapping != 0) {
                        std::cerr << "Ignoring mapping" << std::endl;
                    }
                    if(uvindex != 0) {
                        std::cerr << "Ignoring uvindex" << std::endl;
                    }
                    if(blend != 0) {
                        std::cerr << "Ignoring blend" << std::endl;
                    }
                    if(op != 0) {
                        std::cerr << "Ignoring aiTextureOp" << std::endl;
                    }
                    if(mapmode != 0) {
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GLWrapFromAiMapMode(mapmode[0]) );
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GLWrapFromAiMapMode(mapmode[1]) );
                    }

                    glBindTexture(GL_TEXTURE_2D, 0);
                }else{
                    std::cerr << "Failed to load texture. Type: " << (int)tt << ", id: " << dt << " path: " << path.C_Str() << std::endl;
                }
            }else{
                std::cerr << "Failed to get texture. Type: " << (int)tt << ", id: " << dt << " path: " << path.C_Str() << std::endl;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////
GLuint GLMesh::LoadGLTextureResource(aiString& path)
{
    GLuint glTex = 0;
    if( path.length > 0 && path.data[0] == '*' ) {
        // Texture embedded in file
        std::stringstream ss( std::string(path.data+1) );
        int sId = -1;
        ss >> sId;
        if( 0 <= sId && sId < (int)m_pScene->mNumTextures ) {
            aiTexture* aiTex = m_pScene->mTextures[sId];
            if( aiTex->mHeight == 0 ) {
                glTex = LoadGLTextureFromArray((unsigned char*)aiTex->pcData, aiTex->mWidth, aiTex->achFormatHint );
            }else{
                // WARNING: Untested code condition!
                glGenTextures(1, &glTex);
                glBindTexture(GL_TEXTURE_2D, glTex);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, aiTex->mWidth, aiTex->mHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, aiTex->pcData );
            }
        }else{
            std::cerr << "Unable to Load embedded texture, bad path: " << path.data << std::endl;
        }
    }else{
        // Texture in file resource
        glTex = LoadGLTextureFromFile(path.data, path.length);
    }

    return glTex;
}

////////////////////////////////////////////////////////////////////////////
void GLMesh::ApplyMaterial( const struct aiMaterial *mtl )
{
    float c[4];

    GLenum fill_mode;
    int ret1, ret2;
    aiColor4D diffuse;
    aiColor4D specular;
    aiColor4D ambient;
    aiColor4D emission;
    float shininess, strength;
    int two_sided;
    int wireframe;
    unsigned int max;

    set_float4( c, 0.8f, 0.8f, 0.8f, 1.0f );
    if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse)){
        color4_to_float4( &diffuse, c);
    }
    c[4]*=m_fAlpha;

    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, c );

    set_float4( c, 0.0f, 0.0f, 0.0f, 1.0f );
    if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular)){
        color4_to_float4(&specular, c);
    }
    c[4]*=m_fAlpha;
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, c );

    set_float4( c, 0.2f, 0.2f, 0.2f, 1.0f );
    if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient)){
        color4_to_float4( &ambient, c );
    }
    c[4]*=m_fAlpha;
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, c );

    set_float4( c, 0.0f, 0.0f, 0.0f, 1.0f );
    if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission)){
        color4_to_float4(&emission, c);
    }
    c[4]*=m_fAlpha;
    glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, c );

    CheckForGLErrors();

    max = 1;
    ret1 = aiGetMaterialFloatArray( mtl, AI_MATKEY_SHININESS, &shininess, &max );
    if( ret1 == AI_SUCCESS ){
        max = 1;
        ret2 = aiGetMaterialFloatArray( mtl, AI_MATKEY_SHININESS_STRENGTH, &strength, &max );
        if(ret2 == AI_SUCCESS){
            glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, shininess * strength );
            CheckForGLErrors();
        }
        else{
            //                    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 1 );
            CheckForGLErrors();
        }
    }
    else {
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
        set_float4(c, 0.0f, 0.0f, 0.0f, 0.0f);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);
        CheckForGLErrors();
    }

    return;
    max = 1;
    if( AI_SUCCESS == aiGetMaterialIntegerArray(
                mtl, AI_MATKEY_ENABLE_WIREFRAME, &wireframe, &max) ){
        fill_mode = wireframe ? GL_LINE : GL_FILL;
    }
    else{
        fill_mode = GL_FILL;
    }
    glPolygonMode( GL_FRONT_AND_BACK, fill_mode );
    CheckForGLErrors();

    max = 1;
    if((AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_TWOSIDED, &two_sided, &max)) && two_sided){
        glDisable(GL_CULL_FACE);
        CheckForGLErrors();
    }
    else{
        glEnable(GL_CULL_FACE);
        CheckForGLErrors();
    }

}

////////////////////////////////////////////////////////////////////////////
void GLMesh::RenderFace( GLenum face_mode, GLenum fill_mode, float fAlpha, const struct aiFace* face,  const struct aiMesh* mesh )
{
    glPolygonMode( GL_FRONT_AND_BACK, fill_mode );
    glBegin( face_mode );
    for( unsigned int ii = 0; ii < face->mNumIndices; ii++ ) {
        int index = face->mIndices[ii];
        if( mesh->mColors[0] != NULL ){
            float *c = (float*)&mesh->mColors[0][index];
            glColor4f( c[0], c[1], c[2], fAlpha*c[3] );
        }
        if( mesh->mTextureCoords[0] != NULL ){
            glTexCoord3fv( &mesh->mTextureCoords[0][index].x );
        }
        if( mesh->mNormals != NULL ){
            glNormal3fv( &mesh->mNormals[index].x );
        }
        glVertex3fv( &mesh->mVertices[index].x );
    }
    glEnd();

    // show normals for debugging
    if( m_bShowMeshNormals ){
        float s = 0.1;
        glBegin( GL_LINES );
        for( unsigned int ii = 0; ii < face->mNumIndices; ii++ ) {
            int index = face->mIndices[ii];
            float* p = &mesh->mVertices[index].x;
            float* n = &mesh->mNormals[index].x;
            glVertex3f( p[0], p[1], p[2] );
            glVertex3f( p[0]+s*n[0], p[1]+s*n[1], p[2]+s*n[2] );
        }
        glEnd();
    }

}

////////////////////////////////////////////////////////////////////////////
void GLMesh::RenderMesh(
        const struct aiMesh* mesh,
        const struct aiMaterial* mtl
        )
{
    ApplyMaterial( mtl );
    if( mesh->mNormals == NULL ) {
        glDisable(GL_LIGHTING);
    } else {
        glEnable(GL_LIGHTING);
    }

    // WARNING: A relatively arbitrary list of texture types to
    // try and load. In the future these should be dealt with properly
    const size_t numTexTypesToLoad = 4;
    static aiTextureType texTypesToLoad[numTexTypesToLoad] = {
        aiTextureType_DIFFUSE, aiTextureType_AMBIENT,
        aiTextureType_EMISSIVE, aiTextureType_LIGHTMAP
    };

    int totaltex = 0;

    for(size_t tti=0; tti < numTexTypesToLoad; ++tti ) {
        const aiTextureType tt = texTypesToLoad[tti];
        const unsigned int numTex = mtl->GetTextureCount(tt);
        totaltex += numTex;
        for(unsigned int dt=0; dt < numTex; ++dt ) {
            aiString path;
            if( mtl->GetTexture(tt, dt, &path) == AI_SUCCESS ) {
                std::map<std::string,GLuint>::iterator ix = m_mapPathToGLTex.find(path.data);
                if( ix != m_mapPathToGLTex.end() ) {
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D, ix->second);
                    // Only bind first one for now.
                    goto endoftextures;
                }
            }
        }
    }
endoftextures:

    for( unsigned int t = 0; t < mesh->mNumFaces; ++t) {
        const struct aiFace* face = &mesh->mFaces[t];
        GLenum face_mode;

        switch(face->mNumIndices) {
            case 1: face_mode = GL_POINTS; break;
            case 2: face_mode = GL_LINES; break;
            case 3: face_mode = GL_TRIANGLES; break;
            default: face_mode = GL_POLYGON; break;
        }
        RenderFace( face_mode, GL_FILL, m_fAlpha, face, mesh );
    }

    if( totaltex > 0 ) {
        glDisable(GL_TEXTURE_2D);
    }
}

////////////////////////////////////////////////////////////////////////////

void GLMesh::RecursiveRender( const struct aiScene *sc, const struct aiNode* nd )
{
    unsigned int n = 0;
    aiMatrix4x4 m = nd->mTransformation;

    // update transform
    aiTransposeMatrix4( &m );
    glPushMatrix();
    glMultMatrixf( &(m.a1) );

    // draw all meshes assigned to this node
    for (; n < nd->mNumMeshes; ++n) {
        const struct aiMesh* mesh = m_pScene->mMeshes[nd->mMeshes[n]];
        RenderMesh( mesh, sc->mMaterials[mesh->mMaterialIndex] );
    }


    // draw all children
    for( n = 0; n < nd->mNumChildren; ++n ) {
        RecursiveRender( sc, nd->mChildren[n] );
    }

    glPopMatrix();
}

} // namespace SceneGraph

