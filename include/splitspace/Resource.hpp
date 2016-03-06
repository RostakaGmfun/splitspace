#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include <string>

namespace splitspace {

enum ResourceType {
    RES_UNKNOWN,
    RES_TEXTURE,
    RES_MATERIAL,
    RES_ENTITY,
    RES_OBJECT,
    RES_MESH,
    RES_SHADER,
    RES_SCENE,
    RES_LIGHT
};

struct ResourceManifest {
    ResourceManifest(ResourceType t): type(t)
    {}
    std::string name;
    ResourceType type;
};

class Engine;
class LogManager;
class RenderManager;
class ResourceManager;

class Resource {
public:
    Resource(Engine *e,ResourceManifest *manifest);
    virtual ~Resource() { }

    virtual bool load() = 0;
    virtual void unload() = 0;

    void incRefCount();
    void decRefCount();
    int getRefCount() const;

    ResourceType getType() const { return m_manifest?m_manifest->type:RES_UNKNOWN; }

protected:
    int m_refCount;
    ResourceManifest *m_manifest;
    LogManager *m_logMan;
    RenderManager *m_renderMan;
    ResourceManager *m_resMan;
};

} // namespace splitspace

#endif // RESOURCE_HPP
