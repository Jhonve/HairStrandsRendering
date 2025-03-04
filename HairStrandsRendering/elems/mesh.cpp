#include "elems/mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "render/buffer_manager.h"

void Mesh::init()
{
    m_render_buffer_mgr = std::make_unique<OpenGLVertexIndexBuffer>();
  
    create_buffers();
}
  
Mesh::~Mesh()
{
    delete_buffers();
}
  
bool Mesh::load(const std::string& filepath)
{
    const uint32_t cMeshImportFlags =
                   aiProcess_CalcTangentSpace |
                   aiProcess_Triangulate |
                   aiProcess_SortByPType |
                   aiProcess_GenNormals |
                   aiProcess_GenUVCoords |
                   aiProcess_GenBoundingBoxes |
                   aiProcess_OptimizeMeshes |
                   aiProcess_ValidateDataStructure;
  
    Assimp::Importer Importer;
  
    const aiScene* pScene = Importer.ReadFile(filepath.c_str(), cMeshImportFlags);
  
    if (pScene && pScene->HasMeshes())
    {
        m_vertex_indices.clear();
        m_vertices.clear();

        auto* mesh = pScene->mMeshes[0];
        aiVector3D mesh_max = mesh->mAABB.mMax;
        aiVector3D mesh_min = mesh->mAABB.mMin;
        aiVector3D mesh_center = (mesh_max + mesh_min) / 2.f;
        float unit_scale = (mesh_max - mesh_min).Length() / 2.f;
        float view_scale = (1.f / unit_scale) * 2.5f;

        for (uint32_t i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vh;
            vh.m_pos = { mesh->mVertices[i].x - mesh_center.x,
                         mesh->mVertices[i].y - mesh_center.y,
                         mesh->mVertices[i].z - mesh_center.z };
            vh.m_normal = { mesh->mNormals[i].x, mesh->mNormals[i].y ,mesh->mNormals[i].z };
            
            vh.m_pos = vh.m_pos * view_scale;
            add_vertex(vh);
        }
  
        for (size_t i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
  
            for (size_t j = 0; j < face.mNumIndices; j++)
                add_vertex_index(face.mIndices[j]);
        }
  
        init();
        return true;
    }
    return false;
}

bool Mesh::reload(const std::string& filepath, const glm::vec3& min_pos, const glm::vec3& max_pos)
{
    const uint32_t cMeshImportFlags =
                   aiProcess_CalcTangentSpace |
                   aiProcess_Triangulate |
                   aiProcess_SortByPType |
                   aiProcess_GenNormals |
                   aiProcess_GenUVCoords |
                   aiProcess_OptimizeMeshes |
                   aiProcess_ValidateDataStructure;
  
    Assimp::Importer Importer;
  
    const aiScene* pScene = Importer.ReadFile(filepath.c_str(), cMeshImportFlags);
  
    if (pScene && pScene->HasMeshes())
    {
        m_vertex_indices.clear();
        m_vertices.clear();

        auto* mesh = pScene->mMeshes[0];
        glm::vec3 strands_center = (min_pos + max_pos) / 2.f;
        float unit_scale = glm::length(max_pos - min_pos) / 2.f;
        float view_scale = (1.f / unit_scale) * 2.5f;

        for (uint32_t i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vh;
            vh.m_pos = { mesh->mVertices[i].x - strands_center.x,
                         mesh->mVertices[i].y - strands_center.y,
                         mesh->mVertices[i].z - strands_center.z };
            vh.m_normal = { mesh->mNormals[i].x, mesh->mNormals[i].y ,mesh->mNormals[i].z };
            
            vh.m_pos = vh.m_pos * view_scale;
            add_vertex(vh);
        }
  
        for (size_t i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
  
            for (size_t j = 0; j < face.mNumIndices; j++)
                add_vertex_index(face.mIndices[j]);
        }
  
        init();
        return true;
    }
    return false;

}
  
void Mesh::create_buffers()
{
    m_render_buffer_mgr->create_buffers(m_vertices, m_vertex_indices);
}
  
void Mesh::delete_buffers()
{
    m_render_buffer_mgr->delete_buffers();
}
  
void Mesh::bind()
{
    m_render_buffer_mgr->bind();
}
  
void Mesh::unbind()
{
    m_render_buffer_mgr->unbind();
}
  
void Mesh::render(bool is_shade)
{
    m_render_buffer_mgr->draw((int) m_vertex_indices.size(), is_shade);
}

void QuadMesh::init()
{
    Vertex v_0;
    v_0.m_pos = glm::vec3(-1.0f, -1.0f, 0.0f);
    v_0.m_uv = glm::vec2(0.0f, 0.0f);
    Vertex v_1;
    v_1.m_pos = glm::vec3(1.0, -1.0, 0.0);
    v_1.m_uv = glm::vec2(1.0f, 0.0f);
    Vertex v_2;
    v_2.m_pos = glm::vec3(1.0, 1.0, 0.0);
    v_2.m_uv = glm::vec2(1.0f, 1.0f);
    Vertex v_3;
    v_3.m_pos = glm::vec3(-1.0, 1.0, 0.0);
    v_3.m_uv = glm::vec2(0.0f, 1.0f);
    add_vertex(v_0);
    add_vertex(v_1);
    add_vertex(v_2);
    add_vertex(v_3);
    add_vertex_index(0);
    add_vertex_index(1);
    add_vertex_index(2);
    add_vertex_index(3);

    m_render_buffer_mgr = std::make_unique<OpenGLQuadIndexBuffer>();
    create_buffers();
}
  
QuadMesh::~QuadMesh()
{
    delete_buffers();
}

void QuadMesh::create_buffers()
{
    m_render_buffer_mgr->create_buffers(m_vertices, m_vertex_indices);
}
  
void QuadMesh::delete_buffers()
{
    m_render_buffer_mgr->delete_buffers();
}
  
void QuadMesh::bind()
{
    m_render_buffer_mgr->bind();
}
  
void QuadMesh::unbind()
{
    m_render_buffer_mgr->unbind();
}
  
void QuadMesh::render()
{
    m_render_buffer_mgr->draw((int) m_vertex_indices.size());
}
