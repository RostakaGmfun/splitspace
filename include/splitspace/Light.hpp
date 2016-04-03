#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <splitspace/Entity.hpp>
#include <glm/vec3.hpp>

namespace splitspace {

enum LightType {
    LIGHT_UNKNOWN,
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
    // cosine of angle for spot light
    float spotLightCutoff;
    float power;
};

class Light: public Entity {
public:
    Light(Engine *e, LightManifest *manifest, Entity *poarent = nullptr);

    virtual void update(float dt);
    virtual bool load();
    virtual void unload();

    const glm::vec3 &getPosition() const;
    const glm::vec3 &getRotation() const;
    const glm::vec3 &getDiffuse() const;
    const glm::vec3 &getSpecular() const;
    const LightType getType() const;
    const float getPower() const;

    static LightType getTypeFromName(const std::string &name);
};

}

#endif // LIGHT_HPP
