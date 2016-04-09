#ifndef MESH_HPP
#define MESH_HPP

#include <splitspace/Resource.hpp>
#include <splitspace/RenderManager.hpp>

namespace splitspace {

struct MeshManifest: public ResourceManifest {
    MeshManifest(): ResourceManifest(RES_MESH)
    {}
    bool loadMaterial;
};

class Mesh: public Resource {
public:
    Mesh(Engine *e, MeshManifest *manifest);

    virtual bool load();
    virtual void unload();

    GLuint getVBO() const { return m_vbo; }
    GLuint getIBO() const { return m_ibo; }
    GLuint getVAO() const { return m_vao; }

    std::size_t getNumVerts() const { return m_numVerts; }

private:
    bool createPlane();
    bool createCube();

private:
    GLuint m_vbo;
    GLuint m_ibo;
    GLuint m_vao;

    std::size_t m_numVerts;
};

} // namepsace splitspace

#endif // MESH_HPP
