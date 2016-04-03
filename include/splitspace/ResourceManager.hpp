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
    bool loadShaderLib(const std::string &name);

    bool loadShaderSupport(const std::string &path);

    bool createSceneManifests(const std::vector<std::string> &names);
    bool createScene(const std::string &name);

    void destroy();

    bool addManifest(ResourceManifest *rm);
    ResourceManifest *getManifest(const std::string &name);
    Resource *loadResource(const std::string &name);
    bool unloadResource(const std::string &name);

    int collectGarbage();

    void logStats();

    std::string getResPath() const {
        return m_resPath;
    }

    std::string getShaderSupport() const {
        return m_shaderSupport;
    }

    std::string getDefaultShader() const {
        return m_defaultShader;
    }

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

    std::string m_shaderSupport;
    std::string m_defaultShader;
};

} // namespace splitspace

#endif // RSOURCE_MANAGER_HPP
