#ifndef _GL_MESH_H_
#define _GL_MESH_H_

#include <SceneGraph/GLHelpers.h>
#include <SceneGraph/GLObject.h>

#include <assimp/assimp.h>
#include <assimp/aiPostProcess.h>
#include <assimp/aiScene.h>

#include <IL/il.h>
#include <IL/ilu.h>
#include <map>

namespace SceneGraph
{

struct GLMeshException : std::exception
{
    GLMeshException(const std::string& description ) throw()
        : m_sWhat(description)
    {
    }

    ~GLMeshException() throw() {}

    const char* what() const throw() {
        return m_sWhat.c_str();
    }

protected:
    std::string m_sWhat;
};

class GLMesh : public GLObject
{

    public:
        ////////////////////////////////////////////////////////////////////////////
        GLMesh()
            : GLObject("Mesh"), m_nDisplayList(-1), m_fScale(1), m_fAlpha(1),
              m_iMeshID(-1), m_bShowMeshNormals(false)
        {
        }

        ////////////////////////////////////////////////////////////////////////////
        GLMesh(const std::string& sMeshFile)
            : GLObject("Mesh"), m_nDisplayList(-1), m_fScale(1), m_fAlpha(1),
              m_iMeshID(-1), m_bShowMeshNormals(false)
        {
            Init(sMeshFile);
        }

        ////////////////////////////////////////////////////////////////////////////
        void Init( const std::string& sMeshFile )
        {
            m_pScene = aiImportFile( sMeshFile.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals );
            if( m_pScene == NULL ){
                throw GLMeshException("Unable to load mesh");
            }else{
                Init(m_pScene);
            }
        }

        ////////////////////////////////////////////////////////////////////////////
        void Init( const struct aiScene* pScene )
        {
            m_pScene = pScene;

            if(m_pScene != NULL ) {
                LoadMeshTextures();
            }
        }


        ////////////////////////////////////////////////////////////////////////////
        virtual void  DrawCanonicalObject()
        {
            if( m_pScene ){
                glPushAttrib(GL_ENABLE_BIT);
                glPushMatrix();
                glScalef(m_fScale,m_fScale,m_fScale);

                if( m_nDisplayList == -1 ){
                    m_nDisplayList = glGenLists(1);
                    glNewList( m_nDisplayList, GL_COMPILE );

                    // Recursively render ai_scene's scenegraph
                    RecursiveRender( m_pScene, m_pScene->mRootNode );
                    glEndList();
                }

                glCallList( m_nDisplayList );

                glPopMatrix();

                glPopAttrib();
            }
        }

        ///////////////////////////
        virtual void ComputeDimensions()
        {
            Eigen::Vector3d min, max;
            aiMesh *pAIMesh;
            aiVector3D *pAIVector;

            for( unsigned int x = 0; x < this->GetScene()->mNumMeshes; x++ ){
                pAIMesh = this->GetScene()->mMeshes[x];
                if ( pAIMesh == NULL )
                    continue;

                for( unsigned int y = 0; y < pAIMesh->mNumVertices; y++ ){
                    pAIVector = &pAIMesh->mVertices[y];
                    if ( pAIVector == NULL ){
                        continue;
                        }

                    if ( ((pAIVector->x * this->GetScale()) < min[0]) && ((pAIVector->y * this->GetScale()) < min[1]) && ((pAIVector->z * this->GetScale()) < min[2]) ){
                        min[0] = pAIVector->x * this->GetScale();
                        min[1] = pAIVector->y * this->GetScale();
                        min[2] = pAIVector->z * this->GetScale();
                    }

                    if ( ((pAIVector->x * this->GetScale()) > max[0]) && ((pAIVector->y * this->GetScale()) > max[1]) && ((pAIVector->z * this->GetScale()) > max[2]) ) {
                        max[0] = pAIVector->x * this->GetScale();
                        max[1] = pAIVector->y * this->GetScale();
                        max[2] = pAIVector->z * this->GetScale();
                    }
                }
            }

            m_Dimensions[0] = max[0] - min[0];
            m_Dimensions[1] = max[1] - min[1];
            m_Dimensions[2] = max[2] - min[2];
        }

        // Getters and setters
        const struct aiScene *GetScene( void ) {
            return m_pScene;
        }

        virtual void select( unsigned int )
        {
            // WARNING: When an instance of GLMesh is selected, it appears that it remains selected forever
            // One way to resolve this is to call 'UnSelect( m_iMeshID )' after doing anything pertaining to selection
            // Hopefully we find a better, more permanent solution soon...

            // UPDATE: Don't know if what's above is still a valid statement...
        }

        Eigen::Vector3d GetDimensions() { return m_Dimensions; }

        float GetScale() { return m_fScale; }
        void SetScale( float flScale ) { m_fScale = flScale; }

protected:
        ////////////////////////////////////////////////////////////////////////////
        GLenum GLWrapFromAiMapMode(aiTextureMapMode mode)
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
        void LoadMeshTextures()
        {
            // Ensure DevIL library is initialised
            static bool firsttime = true;
            if(firsttime) {
                ilInit();
                iluInit();
                firsttime = false;
            }

            // For each material, find associated textures
            for (unsigned int m=0; m < m_pScene->mNumMaterials; ++m) {
                LoadMaterialTextures(m_pScene->mMaterials[m]);
            }
        }

        ////////////////////////////////////////////////////////////////////////////
        void LoadMaterialTextures(aiMaterial* pMaterial)
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
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

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
                        }
                    }
                }
            }
        }

        ////////////////////////////////////////////////////////////////////////////
        GLuint LoadGLTextureResource(aiString& path)
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
                        std::ofstream of("test.dat");
                        of.write((char*)aiTex->pcData, aiTex->mWidth);
                        of.close();
                        glTex = LoadGLTextureFromArray((unsigned char*)aiTex->pcData, aiTex->mWidth, aiTex->achFormatHint );
                    }else{
                        // WARNING: Untested code condition!
                        glGenTextures(1, &glTex);
                        glBindTexture(GL_TEXTURE_2D, glTex);
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, aiTex->mWidth, aiTex->mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, aiTex->pcData );
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
        GLuint LoadGLTextureFromDevIL(ILuint ilTexId)
        {
            ilBindImage(ilTexId);

            ILinfo ilImageInfo;
            iluGetImageInfo(&ilImageInfo);

            if (ilImageInfo.Origin == IL_ORIGIN_UPPER_LEFT) {
                iluFlipImage();
            }

            if ( !ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE) ) {
                ILenum ilError = ilGetError();
                std::cerr << "Unable to decode image, DevIL: " << ilError << " - " << iluErrorString(ilError) << std::endl;
                return 0;
            }

            GLuint glTexId = 0;
            glGenTextures(1, &glTexId);
            glBindTexture(GL_TEXTURE_2D, glTexId);
            glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),
                         0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData() );
            return glTexId;
        }

        ////////////////////////////////////////////////////////////////////////////
        GLuint LoadGLTextureFromFile(const char* path, size_t length)
        {
            std::string filename(path);
            if(length >= 2 && filename[0] == '/' && filename[0] == '/' ) {
                filename[0] = '.';
            }

            ILuint ilTexId;
            ilGenImages(1, &ilTexId);
            ilBindImage(ilTexId);

            GLuint glTexId = 0;
            if( ilLoadImage(filename.c_str() ) ) {
                glTexId = LoadGLTextureFromDevIL(ilTexId);
            }else{
                std::cerr << "Failed to load texture file '" << filename << "'" << std::endl;
            }

            ilDeleteImages(1, &ilTexId);

            return glTexId;
        }

        ////////////////////////////////////////////////////////////////////////////
        GLuint LoadGLTextureFromArray(const unsigned char* data, size_t bytes, const char* extensionHint = 0 )
        {
            ILuint ilTexId;
            ilGenImages(1, &ilTexId);
            ilBindImage(ilTexId);

            GLuint glTexId = 0;

            ILenum filetype = IL_TYPE_UNKNOWN;

            // Guess filetype by data
            if (filetype == IL_TYPE_UNKNOWN) {
                filetype = ilDetermineTypeL(data, bytes);
            }

            // Guess filetype by extension
            if( filetype == IL_TYPE_UNKNOWN && extensionHint != 0 ) {
                const std::string sExt(extensionHint);
                const std::string sDummy = std::string("dummy.") + sExt;
                filetype = ilDetermineType(sDummy.c_str());
            }

            // Some extra guesses that seem to be missed in devIL
            if( filetype == IL_TYPE_UNKNOWN ) {
                if( !strcmp(extensionHint, "bmp") ) {
                    filetype = IL_TGA;
                }
            }

            // Load image to ilTexId us devIL
            if( ilLoadL(filetype, data, bytes) ) {
                glTexId = LoadGLTextureFromDevIL(ilTexId);
            }else{
                ILenum ilError = ilGetError();
                std::cerr << "Failed to Load embedded texture, DevIL: " << ilError << " - " << iluErrorString(ilError) << std::endl;
            }

            ilDeleteImages(1, &ilTexId);

            return glTexId;
        }

        ////////////////////////////////////////////////////////////////////////////
        void color4_to_float4( const struct aiColor4D *c, float f[4] )
        {
            f[0] = c->r;
            f[1] = c->g;
            f[2] = c->b;
            f[3] = c->a;
        }

        ////////////////////////////////////////////////////////////////////////////
        void set_float4(float f[4], float a, float b, float c, float d)
        {
            f[0] = a;
            f[1] = b;
            f[2] = c;
            f[3] = d;
        }


        ////////////////////////////////////////////////////////////////////////////
        void ApplyMaterial( const struct aiMaterial *mtl )
        {

            //            glShadeModel( GL_FLAT );
            glDisable( GL_COLOR_MATERIAL );                                 // activate material

            float c[4];

            GLenum fill_mode;
            int ret1, ret2;
            struct aiColor4D diffuse;
            struct aiColor4D specular;
            struct aiColor4D ambient;
            struct aiColor4D emission;
            float shininess, strength;
            int two_sided;
            int wireframe;
            unsigned int max;

            set_float4( c, 0.8f, 0.8f, 0.8f, 1.0f );
            if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse)){
                color4_to_float4( &diffuse, c);
                //    printf("Applying GL_DIFFUSE %f, %f, %f, %f\n", c[0], c[1], c[2], c[3] );
            }
            glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, c );

            set_float4( c, 0.0f, 0.0f, 0.0f, 1.0f );
            if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular)){
                color4_to_float4(&specular, c);
                //    printf("Applying GL_SPECULAR %f, %f, %f, %f\n", c[0], c[1], c[2], c[3] );
            }
            glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, c );

            set_float4( c, 0.2f, 0.2f, 0.2f, 1.0f );
            if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient)){
                color4_to_float4( &ambient, c );
                //   printf("Applying GL_AMBIENT %f, %f, %f, %f\n", c[0], c[1], c[2], c[3] );
            }
            glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, c );

            set_float4( c, 0.0f, 0.0f, 0.0f, 1.0f );
            if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission)){
                color4_to_float4(&emission, c);
                //    printf("Applying GL_EMISSION %f, %f, %f, %f\n", c[0], c[1], c[2], c[3] );
            }
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
        void RenderFace( GLenum face_mode, GLenum fill_mode, float fAlpha, const struct aiFace* face,  const struct aiMesh* mesh )
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
                    //       printf( "Normal %f, %f, %f\n", mesh->mNormals[index].x,
                    //            mesh->mNormals[index].y, mesh->mNormals[index].z );
                    //                    glNormal3f( -mesh->mNormals[index].x, -mesh->mNormals[index].y, -mesh->mNormals[index].z );
                }
                glVertex3fv( &mesh->mVertices[index].x );
            }
            glEnd();

            // show normals for debugging
            if( m_bShowMeshNormals ){
                float s = 10;
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
        void RenderMesh(
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
                            break;
                        }
                    }
                }
            }

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
        void RecursiveRender( const struct aiScene *sc, const struct aiNode* nd )
        {
            unsigned int n = 0;
            struct aiMatrix4x4 m = nd->mTransformation;

            // update transform
            aiTransposeMatrix4( &m );
            glPushMatrix();
            glMultMatrixf( (float*)&m );

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

        const struct aiScene*   m_pScene;
        GLint                   m_nDisplayList;
        float                   m_fScale;
        float                   m_fAlpha; // render translucent meshes?
        unsigned int            m_iMeshID;
        bool                    m_bShowMeshNormals;
        Eigen::Vector3d         m_Dimensions;
        std::map<std::string,GLuint> m_mapPathToGLTex;
};

} // SceneGraph

#endif

