#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <splitspace/Resource.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <splitspace/RenderManager.hpp>

namespace splitspace {

struct TextureManifest;

class Texture;

struct MaterialManifest: public ResourceManifest {
    MaterialManifest(): ResourceManifest(RES_MATERIAL)
    {}
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
public:
    Material(Engine *e, MaterialManifest *manifest);
    virtual ~Material() { unload(); }

    virtual bool load();
    virtual void unload();

private:
    Texture *m_diffuseMap;
    Texture *m_normalMap;
};

} // namespace splitspace

#endif // MATERIAL_HPP
