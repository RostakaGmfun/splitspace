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

}
