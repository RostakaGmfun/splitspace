#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <splitspace/Entity.hpp>
#include <glm/vec3.hpp>

namespace splitspace {

enum LightType {
    LIGHT_AMBIENT,
    LIGHT_SUN,
    LIGHT_POINT,
    LIGHT_SPOT
};

struct LightManifest: public EntityManifest {
    LightManifest(): EntityManifest(RES_LIGHT)
    {}
    LightType lightType;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

class Light: public Entity {
public:
    Light(Engine *e, LightManifest *manifest, Entity *poarent = nullptr);
    virtual ~Light() { unload(); }

    virtual void update(float dt);
    virtual bool load();
    virtual void unload();

    const glm::vec3 &getPosition() const;
    const glm::vec3 &getRotation() const;
    const glm::vec3 &getDiffuse() const;
    const glm::vec3 &getSpecular() const;
    const LightType getType() const;
};

}

#endif // LIGHT_HPP
