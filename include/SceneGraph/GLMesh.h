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

        void Init( const std::string& sMeshFile,
                   bool bFlipUVs = false );
        void Init( const struct aiScene* pScene );

        bool Mouse(int /*button*/, const Eigen::Vector3d& /*win*/,
                   const Eigen::Vector3d& /*obj*/,
                   const Eigen::Vector3d& /*normal*/,
                   bool /*pressed*/, int /*button_state*/, int /*pickId*/) {
          return false;
        }

        bool MouseMotion(const Eigen::Vector3d& /*win*/,
                         const Eigen::Vector3d& /*obj*/,
                         const Eigen::Vector3d& /*normal*/,
                         int /*button_state*/, int /*pickId*/) {
          return false;
        }

        virtual void ComputeDimensions();
        virtual void  DrawCanonicalObject();

        void ShowNormals(bool showNormals = true);

        void ComputeNodeBounds( const struct aiScene *pAIScene, const struct aiNode *pAINode, AxisAlignedBoundingBox& aabb, aiMatrix4x4 dParentTransform );

        void SetScale(Eigen::Vector3d s);

        void SetScale(double s);

        void SetAlpha(const double a) { m_fAlpha = a; }

        void SetMeshColor(SceneGraph::GLColor& mesh_color);

        bool IsSelectable();
        // TODO: Ideally we would deprecate this and not make
        // ASSIMP structures public.
        const struct aiScene *GetScene( void );

        Eigen::Vector3d GetDimensions();

        struct Vertex
        {
            float m_posX;
            float m_posY;
            float m_posZ;

            float m_normalX;
            float m_normalY;
            float m_normalZ;

            float m_texU;
            float m_texV;

            Vertex() {}

            Vertex(const float posX, const float posY, const float posZ,
                   const float texU, const float texV,
                   const float normX,const float normY,const float normZ)
            {
                m_posX = posX;
                m_posY = posY;
                m_posZ = posZ;

                m_texU = texU;
                m_texV = texV;

                m_normalX = normX;
                m_normalY = normY;
                m_normalZ = normZ;
            }
        };

        struct Mesh {
            Mesh();

            ~Mesh();

            void Init(const std::vector<Vertex>& vVertices,
                      const std::vector<unsigned int>& vIndices,
                      const aiMatrix4x4& m_Transformation);

            GLuint m_uVB;
            GLuint m_uIB;
            aiMatrix4x4 m_Transformation;
            unsigned int m_uNumIndices;
            unsigned int m_uMaterialIndex;
        };

        void InitMesh(unsigned int Index, const aiMesh *paiMesh, const aiMatrix4x4 &mTransformation);
        void InitNode(const aiScene *sc, const aiNode *nd, const aiMatrix4x4 &mTransformation);
protected:
        void LoadMeshTextures();
        void LoadMaterialTextures(aiMaterial* pMaterial);
        GLuint LoadGLTextureResource(aiString& path);
        void ApplyMaterial(const struct aiMaterial *mtl );
        
//        void RecursiveRender( const struct aiScene *sc, const struct aiNode* nd );
//        void RenderMesh( const struct aiMesh* mesh, const struct aiMaterial* mtl );
//        void RenderFace( GLenum face_mode, GLenum fill_mode, float fAlpha, const struct aiFace* face,  const struct aiMesh* mesh );


        const struct aiScene*   m_pScene;
        SceneGraph::GLColor     m_meshcolor;
        float                   m_fAlpha; // render translucent meshes?
        unsigned int            m_iMeshID;
        bool                    m_bShowMeshNormals;
        unsigned int            m_uMeshCount;
        std::map<std::string,GLuint> m_mapPathToGLTex;
        std::vector<Mesh> m_Meshes;
};

} // SceneGraph

#endif

