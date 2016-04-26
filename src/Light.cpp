#include <splitspace/Light.hpp>

namespace splitspace {

Light::Light(Engine *e, LightManifest *manifest, Entity *parent):
                                                 Entity(e, manifest, parent)
{}

bool Light::load() {
    Entity::load();
    LightManifest *lm = static_cast<LightManifest*>(m_manifest);
    m_diffuse = lm->diffuse;
    m_specular = lm->specular;
    m_lightType = lm->lightType;
    m_power = lm->power;
    m_attenuation = lm->attenuation;
    m_spotLightCutoff = lm->spotLightCutoff;
    m_isLoaded = true;
    return true;
}

void Light::unload() {
    m_isLoaded = false;
}

void Light::update(float dt) {
    Entity::update(dt);
}

LightType Light::getTypeFromName(const std::string &name) {
    if(name == "ambient") {
        return LIGHT_AMBIENT;
    } else if(name == "sun") {
        return LIGHT_SUN;
    } else if(name == "point") {
        return LIGHT_POINT;
    } else if(name == "spot") {
        return LIGHT_SPOT;
    } else {
        return LIGHT_UNKNOWN;
    }
}

}
