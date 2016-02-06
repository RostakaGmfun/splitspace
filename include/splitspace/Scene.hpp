#ifndef SCENE_HPP
#define SCENE_HPP

#include <splitspace/Resource.hpp>

#include <vector>

namespace splitspace {

class Entity;
class SceneManager;
class ResourceManager;

struct ObjectManifest;
struct LightManifest;

struct SceneManifest: public ResourceManifest {
    std::vector<ObjectManifest *> objects;
    std::vector<LightManifest *> lights;
};

class Scene: public Resource {
public:
    Scene(Engine *e, SceneManifest *manifest);
    virtual ~Scene();

    virtual bool load();
    virtual void unload();

    Entity *getRootNode() const { return m_rootNode; }

private:
    Entity *m_rootNode;
    SceneManager *m_sceneManager;
    ResourceManager *m_resManager;
    Engine *m_engine;
};

} // namespace splitspace

#endif // SCENE_HPP
