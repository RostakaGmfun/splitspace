#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include <string>

namespace splitspace {

struct ResourceManifest {
    std::string name;
};

class Engine;
class LogManager;
class RenderManager;

class Resource {
public:
    Resource(Engine *e,ResourceManifest *manifest);
    virtual ~Resource() { }

    virtual bool load() = 0;
    virtual void unload() = 0;

    void incRefCount();
    void decRefCount();
    int getRefCount() const;

protected:
    int m_refCount;
    ResourceManifest *m_manifest;
    LogManager *m_logMan;
    RenderManager *m_renderMan;
};

} // namespace splitspace

#endif // RESOURCE_HPP
