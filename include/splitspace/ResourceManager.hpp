#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <string>
#include <map>
#include <vector>

namespace splitspace {

class Engine;
class LogManager;

class Resource;
class Texture;
class Shader;
class Mesh;
class Material;
class Object;
class Scene;

struct ResourceManifest;
struct TextureManifest;
struct ObjectManifest;
struct MeshManifest;
struct MaterialManifest;
struct ShaderManifest;
struct SceneManifest;

class ResourceManager {
public:
    ResourceManager(Engine *e);
    ~ResourceManager();

    bool loadMaterialLib(std::vector<std::string> ml);
    bool loadMaterialLib(std::string name);

    bool createSceneManifests(std::vector<std::string> names);
    bool createScene(std::string name);

    void destroy();

    bool addManifest(ResourceManifest *rm);
    ResourceManifest *getManifest(std::string name);
    Resource *loadResource(std::string name);
    void unloadResource(std::string name);

    int collectGarbage();

private:
    TextureManifest *readTextureManifest(std::string name);

private:
    Engine *m_engine;
    LogManager *m_logMan;

    std::map<std::string, Resource *> m_resourceCache;
    std::map<std::string, ResourceManifest *> m_resourceManifests;
};

} // namespace splitspace

#endif // RSOURCE_MANAGER_HPP
