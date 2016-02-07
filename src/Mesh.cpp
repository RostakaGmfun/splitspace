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

    std::string path = "data/meshes/"+m_manifest->name;

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
        " contains more than 1 mesh, onle first will be imported");
    }

    return true;
}

void Mesh::unload() {
    decRefCount();
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

    return m_renderMan->createMesh(verts, 6, m_vbo, m_vao);
}

bool Mesh::createCube() {
    return true;
}

} // namespace splitspace
