#ifndef _ASSET_IMPORTER_H_
#define _ASSET_IMPORTER_H_

// assimp include files. These three are usually needed.
#include "assimp/assimp.h"
#include "assimp/aiPostProcess.h"
#include "assimp/aiScene.h"


#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)

class AssetLoader
{

    public:

        //////////////////////////////////////////////////////////////////////////////
        AssetLoader()
        {
            m_pScene = NULL;

            // get a handle to the predefined STDOUT log stream and attach
            // it to the logging system. It remains active for all further
            // calls to aiImportFile(Ex) and aiApplyPostProcessing.
            stream = aiGetPredefinedLogStream( aiDefaultLogStream_STDOUT, NULL );
            aiAttachLogStream( &stream );

            // ... same procedure, but this stream now writes the
            // log messages to assimp_log.txt
            stream = aiGetPredefinedLogStream( aiDefaultLogStream_FILE, "assimp_log.txt" );
            aiAttachLogStream( &stream );
        }

        //////////////////////////////////////////////////////////////////////////////
        ~AssetLoader()
        {
            // cleanup - calling 'aiReleaseImport' is important, as the library 
            // keeps internal resources until the scene is freed again. Not 
            // doing so can cause severe resource leaking.
            aiReleaseImport( m_pScene);

            // We added a log stream to the library, it's our job to disable it
            // again. This will definitely release the last resources allocated
            // by Assimp.
            aiDetachAllLogStreams();
        }

        //////////////////////////////////////////////////////////////////////////////
        bool Load( const std::string& sName )
        {
            const char* path = sName.c_str();

            // we are taking one of the postprocessing presets to avoid
            // spelling out 20+ single postprocessing flags here.
            m_pScene = aiImportFile( path, aiProcessPreset_TargetRealtime_MaxQuality );

/*
            if( m_pScene ){
                get_bounding_box( &scene_min, &scene_max );
                return true;
            }
*/
            return false;
        }

/*
        //////////////////////////////////////////////////////////////////////////////
        void get_bounding_box_for_node (
                const struct aiNode* nd, 
                struct aiVector3D* min, 
                struct aiVector3D* max, 
                struct aiMatrix4x4* trafo
                )
        {
            struct aiMatrix4x4 prev;
            unsigned int n = 0, t;

            prev = *trafo;
            aiMultiplyMatrix4( trafo, &nd->mTransformation );

            for (; n < nd->mNumMeshes; ++n) {
                const struct aiMesh* mesh = m_pScene->mMeshes[ nd->mMeshes[n] ];
                for (t = 0; t < mesh->mNumVertices; ++t) {

                    struct aiVector3D tmp = mesh->mVertices[t];
                    aiTransformVecByMatrix4(&tmp,trafo);

                    min->x = aisgl_min(min->x,tmp.x);
                    min->y = aisgl_min(min->y,tmp.y);
                    min->z = aisgl_min(min->z,tmp.z);

                    max->x = aisgl_max(max->x,tmp.x);
                    max->y = aisgl_max(max->y,tmp.y);
                    max->z = aisgl_max(max->z,tmp.z);
                }
            }

            for (n = 0; n < nd->mNumChildren; ++n) {
                get_bounding_box_for_node( nd->mChildren[n], min, max, trafo );
            }
            *trafo = prev;
        }

        // ----------------------------------------------------------------------------
        void get_bounding_box (struct aiVector3D* min, struct aiVector3D* max)
        {
            struct aiMatrix4x4 trafo;
            aiIdentityMatrix4( &trafo );

            min->x = min->y = min->z =  1e10f;
            max->x = max->y = max->z = -1e10f;
            get_bounding_box_for_node( m_pScene->mRootNode, min, max, &trafo );
        }
        */


    private:
        const struct aiScene*         m_pScene;
        struct aiLogStream      stream;

};

#endif

