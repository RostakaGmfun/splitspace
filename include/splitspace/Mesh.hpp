#ifndef MESH_HPP
#define MESH_HPP

#include <splitspace/Resource.hpp>
#include <GL/gl.h>

namespace splitspace {

struct MeshManifest: public ResourceManifest {

};

class Mesh: public Resource {
public:
    Mesh(Engine *e, MeshManifest *manifest);
    virtual ~Mesh() { unload(); }

    virtual bool load();
    virtual void unload();

private:
    GLuint m_vbo;
    GLuint m_ibo;
};

} // namepsace splitspace

#endif // MESH_HPP
