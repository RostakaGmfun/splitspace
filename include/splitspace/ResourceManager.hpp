#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <string>
#include <map>
#include <vector>

namespace splitspace {

class Engine;
class LogManager;

class Resource;

struct ResourceManifest;
struct TextureManifest;

class ResourceManager {
public:
    ResourceManager(Engine *e, const std::string &resPath = "data/");
    ~ResourceManager();

    bool loadMaterialLib(const std::vector<std::string> &ml);
    bool loadMaterialLib(const std::string &name);

    bool createSceneManifests(const std::vector<std::string> &names);
    bool createScene(const std::string &name);

    void destroy();

    bool addManifest(ResourceManifest *rm);
    ResourceManifest *getManifest(const std::string &name);
    Resource *loadResource(const std::string &name);
    bool unloadResource(const std::string &name);

    int collectGarbage();

    void logStats();

private:
    TextureManifest *readTextureManifest(const std::string &name);

private:
    Engine *m_engine;
    LogManager *m_logMan;

    std::map<std::string, Resource *> m_resourceCache;
    std::map<std::string, ResourceManifest *> m_resourceManifests;

    int m_totalResLoaded;
    int m_totalResFails;

    std::string m_resPath;
};

} // namespace splitspace

#endif // RSOURCE_MANAGER_HPP
