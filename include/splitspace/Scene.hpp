#ifndef SCENE_HPP
#define SCENE_HPP

#include <splitspace/Resource.hpp>

#include <vector>
#include <map>

namespace splitspace {

class Entity;
class SceneManager;
class ResourceManager;
class Object;
class Material;

struct ObjectManifest;
struct LightManifest;

struct SceneManifest: public ResourceManifest {
    SceneManifest(): ResourceManifest(RES_SCENE)
    {}
    std::vector<ObjectManifest *> objects;
    std::vector<LightManifest *> lights;
};

typedef std::map<const Material *, std::vector<const Object *> > RenderMap;

class Scene: public Resource {
public:
    Scene(Engine *e, SceneManifest *manifest);

    virtual bool load();
    virtual void unload();

    const Entity *getRootNode() const { return m_rootNode; }

    const RenderMap &getRenderMap() const { return m_renderMap; }

private:
    Entity *m_rootNode;
    ResourceManager *m_resManager;
    Engine *m_engine;

    RenderMap m_renderMap;
};

} // namespace splitspace

#endif // SCENE_HPP
