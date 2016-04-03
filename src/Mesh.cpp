#include <splitspace/Mesh.hpp>
#include <splitspace/LogManager.hpp>
#include <splitspace/ResourceManager.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace splitspace {

Mesh::Mesh(Engine *e, MeshManifest *manifest): Resource(e, manifest),
                                               m_vbo(0),
                                               m_ibo(0)
{}

bool Mesh::load() {
    if(!m_manifest) {
        m_logMan->logErr("(Mesh) No manifest specified");
        return false;
    }

    if(m_manifest->name.empty()) {
        m_logMan->logErr("(Mesh) Empty resource names not supported");
        return false;
    }

    if(m_manifest->name == "__plane__") {
        return createPlane();
    }

    if(m_manifest->name == "__cube__") {
        return createCube();
    }
         
    Assimp::Importer importer;

    std::string path = m_resMan->getResPath()+"meshes/"+m_manifest->name;

    const aiScene *scene = importer.ReadFile(path.c_str(),
            aiProcess_CalcTangentSpace  |
            aiProcess_Triangulate |
            aiProcess_SortByPType);

    if(!scene) {
        m_logMan->logErr("(Mesh) Error importing "+path);
        m_logMan->logErr("(Mesh)\t"+std::string(importer.GetErrorString()));
        return false;
    }
    
    if(scene->mNumMeshes>1) {
        m_logMan->logWarn("(Mesh) "+path+
        " contains more than 1 mesh, only first will be imported");
    }

    aiMesh *mesh = scene->mMeshes[0];

    VertexFormat format;

    format = VERTEX_3DN;

    if(mesh->HasTextureCoords(0)) {
        format = VERTEX_3DTN;
    }

    int numVerts = mesh->mNumVertices;
    void *vertexData = nullptr;
    switch(format) {
        case VERTEX_3DT: {
            Vertex3DT *verts = new Vertex3DT[numVerts];
            for(int i = 0;i<numVerts;i++) {
                verts[i].pos = glm::vec3(mesh->mVertices[i].x,
                                         mesh->mVertices[i].y,
                                         mesh->mVertices[i].z);
                verts[i].texcoord = glm::vec2(mesh->mTextureCoords[0][i].x,
                                              mesh->mTextureCoords[0][i].y);
            }
            vertexData = verts;
        break;
        }
        case VERTEX_3DN: {
            Vertex3DN *verts = new Vertex3DN[numVerts];
            for(int i = 0;i<numVerts;i++) {
                verts[i].pos = glm::vec3(mesh->mVertices[i].x,
                                         mesh->mVertices[i].y,
                                         mesh->mVertices[i].z);
                verts[i].normal = glm::vec3(mesh->mVertices[i].x,
                                            mesh->mVertices[i].y,
                                            mesh->mVertices[i].z);
            }
            vertexData = verts;
        break;
        }
        case VERTEX_3DTN: {
            Vertex3DTN *verts = new Vertex3DTN[numVerts];
            for(int i = 0;i<numVerts;i++) {
                verts[i].pos = glm::vec3(mesh->mVertices[i].x,
                                         mesh->mVertices[i].y,
                                         mesh->mVertices[i].z);
                verts[i].texcoord = glm::vec2(mesh->mTextureCoords[0][i].x,
                                              1-mesh->mTextureCoords[0][i].y);
                verts[i].texcoord*=0.5;
                verts[i].normal = glm::vec3(mesh->mVertices[i].x,
                                            mesh->mVertices[i].y,
                                            mesh->mVertices[i].z);
            }
            vertexData = verts;
        break;
        }
        default:
            m_logMan->logErr("["+path+"]Wrong vertex format");
            return false;
    }

    if(!m_renderMan->createMesh(vertexData, format, numVerts, m_vbo, m_vao)) {
        m_logMan->logErr("["+path+"] Failed to create mesh");
        return false;
    }

    m_isLoaded = true;
    return true;
}

void Mesh::unload() {
    m_logMan->logInfo("(Mesh) Unloading "+m_manifest->name);
    m_renderMan->destroyMesh(m_vao, m_vbo);
    m_isLoaded = false;
}

bool Mesh::createPlane() {
    using namespace glm;
    static const Vertex3DTN verts[] = {
        { vec3(-0.5,0,-0.5), vec2(0,1), vec3(0,1,0) },
        { vec3(0.5,0,0.5), vec2(1,0), vec3(0,1,0) },
        { vec3(-0.5,0,0.5), vec2(0,0), vec3(0,1,0) },

        { vec3(-0.5,0,-0.5), vec2(0,1), vec3(0,1,0) },
        { vec3(0.5,0,-0.5), vec2(1,1), vec3(0,1,0) },
        { vec3(0.5,0,0.5), vec2(1,0), vec3(0,1,0) }
    };

    return m_renderMan->createMesh(verts, VERTEX_3DTN, 6, m_vbo, m_vao);
}

bool Mesh::createCube() {
    //TODO
    return true;
}

} // namespace splitspace
