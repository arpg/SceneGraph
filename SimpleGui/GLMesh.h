#ifndef _GL_MESH_H_
#define _GL_MESH_H_

#include <SimpleGui/GLObject.h>

#include <assimp/assimp.h>
#include <assimp/aiPostProcess.h>
#include <assimp/aiScene.h>

//#define aisgl_min(x,y) (x<y?x:y)
//#define aisgl_max(x,y) (y>x?y:x)

class GLMesh : public GLObject
{

    public:
        GLMesh()
        {
            m_sObjectName = "Mesh";
        }

        ////////////////////////////////////////////////////////////////////////////
        void Init( const struct aiScene* pScene )
        {
            m_pScene = pScene;
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
        void apply_material(const struct aiMaterial *mtl)
        {
            /*
            GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
            GLfloat mat_ambient[] = { 0.7, 0.7, 0.7, 1.0 };
            GLfloat mat_ambient_color[] = { 0.8, 0.8, 0.2, 1.0 };
            GLfloat mat_diffuse[] = { 0.1, 0.5, 0.8, 1.0 };
            GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
            GLfloat no_shininess[] = { 0.0 };
            GLfloat low_shininess[] = { 5.0 };
            GLfloat high_shininess[] = { 100.0 };
            GLfloat mat_emission[] = {0.3, 0.2, 0.2, 0.0};

            glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
            glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
            glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
            return;
            */


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
//                printf("Applying GL_DIFFUSE %f, %f, %f, %f\n", c[0], c[1], c[2], c[3] );
            }
            glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, c );

            set_float4( c, 0.0f, 0.0f, 0.0f, 1.0f );
            if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular)){
                color4_to_float4(&specular, c);
//                printf("Applying GL_SPECULAR %f, %f, %f, %f\n", c[0], c[1], c[2], c[3] );
            }
            glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, c );

            set_float4( c, 0.2f, 0.2f, 0.2f, 1.0f );
            if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient)){
                color4_to_float4( &ambient, c );
 //               printf("Applying GL_AMBIENT %f, %f, %f, %f\n", c[0], c[1], c[2], c[3] );
            }
            glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, c );

            set_float4( c, 0.0f, 0.0f, 0.0f, 1.0f );
            if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission)){
                color4_to_float4(&emission, c);
//                printf("Applying GL_EMISSION %f, %f, %f, %f\n", c[0], c[1], c[2], c[3] );
            }
            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, c);

    
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
            glPolygonMode(GL_FRONT_AND_BACK, fill_mode);
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
        void RenderMesh( GLenum face_mode, GLenum fill_mode, float fAlpha, const struct aiFace* face,  const struct aiMesh* mesh )
        {
            glPolygonMode( GL_FRONT_AND_BACK, fill_mode );
            glBegin( face_mode );
            for( unsigned int ii = 0; ii < face->mNumIndices; ii++ ) {
                int index = face->mIndices[ii];
                if( mesh->mColors[0] != NULL ){
                    float *c = (float*)&mesh->mColors[0][index];
                    glColor4f( c[0], c[1], c[2], fAlpha*c[3] );
                }
                if( mesh->mNormals != NULL ){
                    glNormal3fv( &mesh->mNormals[index].x );
                }
                glVertex3fv( &mesh->mVertices[index].x );
            }
            glEnd();

        }

        ////////////////////////////////////////////////////////////////////////////
        void recursive_render( const struct aiScene *sc, const struct aiNode* nd )
        {
            unsigned int n = 0, t;
            struct aiMatrix4x4 m = nd->mTransformation;

            // update transform
            aiTransposeMatrix4( &m );
            glPushMatrix();
            glMultMatrixf( (float*)&m );

            // draw all meshes assigned to this node
            for (; n < nd->mNumMeshes; ++n) {
                const struct aiMesh* mesh = m_pScene->mMeshes[nd->mMeshes[n]];

                apply_material( sc->mMaterials[mesh->mMaterialIndex] );

                if( mesh->mNormals == NULL ) {
                    glDisable(GL_LIGHTING);
                } else {
                    glEnable(GL_LIGHTING);
                }

                for (t = 0; t < mesh->mNumFaces; ++t) {
                    const struct aiFace* face = &mesh->mFaces[t];
                    GLenum face_mode;

                    switch(face->mNumIndices) {
                        case 1: face_mode = GL_POINTS; break;
                        case 2: face_mode = GL_LINES; break;
                        case 3: face_mode = GL_TRIANGLES; break;
                        default: face_mode = GL_POLYGON; break;
                    }
                    RenderMesh( face_mode, GL_FILL, 1, face, mesh );
                }

                /*
//                glDisable(GL_LIGHTING);
//                glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
//                glEnable (GL_BLEND );
                for (t = 0; t < mesh->mNumFaces; ++t) {
                    const struct aiFace* face = &mesh->mFaces[t];
                    GLenum face_mode;
                    switch(face->mNumIndices) {
                        case 1: face_mode = GL_POINTS; break;
                        case 2: face_mode = GL_LINES; break;
                        case 3: face_mode = GL_TRIANGLES; break;
                        default: face_mode = GL_POLYGON; break;
                    }
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
                    glBegin( face_mode );
                    for( int i = 0; i < face->mNumIndices; i++) {
                        int index = face->mIndices[i];
                        //glColor4f( 1.0, 1.0, 1.0, 0.2 );
                        glVertex3fv(&mesh->mVertices[index].x);
                    }
                    glEnd();

                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
                    glBegin( face_mode );
                    for( int i = 0; i < face->mNumIndices; i++) {
                        int index = face->mIndices[i];
                        glColor4f( 1.0, 1.0, 1.0, 1 );
                        glVertex3fv(&mesh->mVertices[index].x);
                    }
                    glEnd();
                }
                    */
            }


            // draw all children
            for( n = 0; n < nd->mNumChildren; ++n ) {
                recursive_render( sc, nd->mChildren[n] );
            }
            glPopMatrix();
        }

        ////////////////////////////////////////////////////////////////////////////
        void  draw()
        {
//            glRotatef( 90, 1,0,0 );
            glColor4f( 1,1,1,1);
            if( m_pScene ){
                glEnable( GL_DEPTH_TEST );
                recursive_render( m_pScene, m_pScene->mRootNode );
            }
        }

        // Getters and setters
        const struct aiScene *GetScene( void ) { return m_pScene; }

    private:
        const struct aiScene*   m_pScene;
};


#endif

