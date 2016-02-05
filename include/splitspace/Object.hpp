#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <splitspace/Entity.hpp>

namespace splitspace {

struct MaterialManifest;
struct MeshManifest;

struct ObjectManifest: public EntityManifest {
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
};

} // namespace splitspace

#endif // OBJECT_HPP
