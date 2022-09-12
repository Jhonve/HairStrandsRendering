#include "elems/mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "render/opengl_buffer_manager.h"

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
                   aiProcess_OptimizeMeshes |
                   aiProcess_ValidateDataStructure;
  
    Assimp::Importer Importer;
  
    const aiScene* pScene = Importer.ReadFile(filepath.c_str(), cMeshImportFlags);
  
    if (pScene && pScene->HasMeshes())
    {
        m_vertex_indices.clear();
        m_vertices.clear();
  
        auto* mesh = pScene->mMeshes[0];
  
        for (uint32_t i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vh;
            vh.m_pos = { mesh->mVertices[i].x, mesh->mVertices[i].y ,mesh->mVertices[i].z };
            vh.m_normal = { mesh->mNormals[i].x, mesh->mNormals[i].y ,mesh->mNormals[i].z };
  
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
  
void Mesh::render()
{
    m_render_buffer_mgr->draw((int) m_vertex_indices.size());
}