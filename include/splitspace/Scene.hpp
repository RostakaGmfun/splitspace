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
class Light;

struct ObjectManifest;
struct LightManifest;

struct SceneManifest: public ResourceManifest {
    SceneManifest(): ResourceManifest(RES_SCENE)
    {}
    std::vector<ObjectManifest *> objects;
    std::vector<LightManifest *> lights;
};

typedef std::map<const Material *, std::vector<const Object *> > RenderMap;
typedef std::vector<const Light *> LightList;

class Scene: public Resource {
public:
    Scene(Engine *e, SceneManifest *manifest);

    virtual bool load();
    virtual void unload();

    void update(float dt);

    Entity *getRootNode() const { return m_rootNode; }

    const RenderMap &getRenderMap() const { return m_renderMap; }
    const LightList &getLightList() const { return m_lightList; }

private:
    void updateRenderMap();

private:
    Entity *m_rootNode;
    ResourceManager *m_resManager;
    Engine *m_engine;

    RenderMap m_renderMap;
    LightList m_lightList;
};

} // namespace splitspace

#endif // SCENE_HPP
