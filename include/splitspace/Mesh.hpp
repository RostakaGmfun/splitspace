#ifndef MESH_HPP
#define MESH_HPP

#include <splitspace/Resource.hpp>
#include <splitspace/RenderManager.hpp>

namespace splitspace {

struct MeshManifest: public ResourceManifest {
    bool loadMaterial;
};

class Mesh: public Resource {
public:
    Mesh(Engine *e, MeshManifest *manifest);
    virtual ~Mesh() { unload(); }

    virtual bool load();
    virtual void unload();

    GLuint getVBO() const { return m_vbo; }
    GLuint getIBO() const { return m_ibo; }
    GLuint getVAO() const { return m_vao; }

private:
    bool createPlane();
    bool createCube();

private:
    GLuint m_vbo;
    GLuint m_ibo;
    GLuint m_vao;
};

} // namepsace splitspace

#endif // MESH_HPP
