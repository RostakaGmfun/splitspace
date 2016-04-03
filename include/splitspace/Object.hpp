#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <splitspace/Entity.hpp>

namespace splitspace {

struct MaterialManifest;
struct MeshManifest;

class Material;
class Mesh;

struct ObjectManifest: public EntityManifest {
    ObjectManifest(): EntityManifest(RES_OBJECT)
    {}
    MaterialManifest *materialManifest;
    MeshManifest *meshManifest;
};

class Object: public Entity {
public:
    Object(Engine *e, ObjectManifest *manifest, Entity *parent = nullptr);

    virtual void update(float dt);

    virtual bool load();
    virtual void unload();

    const Mesh *getMesh() const { return m_mesh; }
    const Material *getMaterial() const { return m_material; }

private:
    Material *m_material;
    Mesh *m_mesh;
};

} // namespace splitspace

#endif // OBJECT_HPP
