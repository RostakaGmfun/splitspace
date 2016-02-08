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
    virtual ~Object() { unload(); }

    virtual void update(float dt);

    virtual bool load();
    virtual void unload();

private:
    Material *m_material;
    Mesh *m_mesh;
};

} // namespace splitspace

#endif // OBJECT_HPP
