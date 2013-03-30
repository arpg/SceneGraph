#ifndef _GL_MESH_H_
#define _GL_MESH_H_

#include <map>
#include <SceneGraph/SceneGraph.h>
#include <assimp/scene.h>

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
        GLMesh();
        GLMesh(const std::string& sMeshFile);
        ~GLMesh();

        void Init( const std::string& sMeshFile, bool bFlipUVs = false );
        void Init( const struct aiScene* pScene );

        virtual void ComputeDimensions();
        virtual void  DrawCanonicalObject();

        void ShowNormals(bool showNormals = true);

        void ComputeNodeBounds( const struct aiScene *pAIScene, const struct aiNode *pAINode, AxisAlignedBoundingBox& aabb, aiMatrix4x4 dParentTransform );

        void SetScale(Eigen::Vector3d s);

        void SetScale(double s);

        void SetAlpha(const double a) { m_fAlpha = a; }

        // TODO: Ideally we would deprecate this and not make
        // ASSIMP structures public.
        const struct aiScene *GetScene( void );

        Eigen::Vector3d GetDimensions();

    protected:
        void LoadMeshTextures();
        void LoadMaterialTextures(aiMaterial* pMaterial);
        GLuint LoadGLTextureResource(aiString& path);
        
        void RecursiveRender( const struct aiScene *sc, const struct aiNode* nd );
        void RenderMesh( const struct aiMesh* mesh, const struct aiMaterial* mtl );
        void RenderFace( GLenum face_mode, GLenum fill_mode, float fAlpha, const struct aiFace* face,  const struct aiMesh* mesh );
        void ApplyMaterial(const struct aiMaterial *mtl );

        const struct aiScene*   m_pScene;
        float                   m_fAlpha; // render translucent meshes?
        unsigned int            m_iMeshID;
        bool                    m_bShowMeshNormals;
        std::map<std::string,GLuint> m_mapPathToGLTex;
};

} // SceneGraph

#endif

