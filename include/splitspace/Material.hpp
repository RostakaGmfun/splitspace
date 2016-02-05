#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <splitspace/Resource.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <splitspace/RenderManager.hpp>

namespace splitspace {

struct TextureManifest;

struct MaterialManifest: public ResourceManifest {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    TextureManifest *diffuseMap;
    TextureManifest *normalMap;
    int repeatX;
    int repeatY;
    TextureFiltering filtering;
    bool mipmappingEnabled;
    TextureUsage usage;

};

class Material: public Resource {
    Material(Engine *e, MaterialManifest *manifest);
    virtual ~Material();

    virtual bool load();
    virtual void unload();
};

} // namespace splitspace

#endif // MATERIAL_HPP
