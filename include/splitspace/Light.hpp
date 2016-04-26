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
    LightManifest(): EntityManifest(RES_LIGHT),
                     attenuation(1,0,0)
    {}
    LightType lightType;
    glm::vec3 diffuse;
    glm::vec3 specular;
    // cosine of angle for spot light
    float spotLightCutoff;
    float power;
    glm::vec3 attenuation;
};

class Light: public Entity {
public:
    Light(Engine *e, LightManifest *manifest, Entity *poarent = nullptr);

    virtual void update(float dt);
    virtual bool load();
    virtual void unload();

    const glm::vec3 &getDiffuse() const { return m_diffuse; }
    const glm::vec3 &getSpecular() const { return m_specular; }
    LightType getType() const { return m_lightType; }
    float getPower() const { return m_power; }
    float getSpotLightCutoff() const { return m_spotLightCutoff; }
    const glm::vec3 &getAttenuation() const { return m_attenuation; }

    void setDiffuse(const glm::vec3 &c) { m_diffuse = c; }
    void setSpecular(const glm::vec3 &c) { m_specular = c;}
    void setPower(float p) { m_power = p; }
    void setAttenuation(const glm::vec3 &a) { m_attenuation = a; }
    void setSpotLightCutoff(float c) { m_spotLightCutoff = c; }

    static LightType getTypeFromName(const std::string &name);

private:
    glm::vec3 m_diffuse;
    glm::vec3 m_specular;
    glm::vec3 m_attenuation;
    float m_power;
    LightType m_lightType;
    float m_spotLightCutoff;
};

}

#endif // LIGHT_HPP
