#include <splitspace/Light.hpp>

namespace splitspace {

Light::Light(Engine *e, LightManifest *manifest, Entity *parent):
                                                 Entity(e, manifest, parent)
{}

void Light::update(float dt) {

}

bool Light::load() {
    return true;
}

void Light::unload() {

}

const glm::vec3 &Light::getPosition() const {
    return static_cast<LightManifest *>(m_manifest)->pos;
}

const glm::vec3 &Light::getRotation() const {
    return static_cast<LightManifest *>(m_manifest)->rot;
}

const glm::vec3 &Light::getDiffuse() const {
    return static_cast<LightManifest *>(m_manifest)->diffuse;
}

const glm::vec3 &Light::getSpecular() const {
    return static_cast<LightManifest *>(m_manifest)->specular;
}

const LightType Light::getType() const {
    return static_cast<LightManifest *>(m_manifest)->lightType;
}

const float Light::getPower() const {
    return static_cast<LightManifest *>(m_manifest)->power;
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
