#include <splitspace/Resource.hpp>
#include <splitspace/Engine.hpp>

#include <assert.h>

namespace splitspace {

Resource::Resource(Engine *e, ResourceManifest *manifest): 
                                                m_refCount(0),
                                                m_isLoaded(false),
                                                m_manifest(manifest),
                                                m_logMan(e->logManager),
                                                m_renderMan(e->renderManager),
                                                m_resMan(e->resManager)
{}

Resource::~Resource() {
    if(m_isLoaded) {
        unload();
    }
}

void Resource::incRefCount() {
    m_refCount++;
}

void Resource::decRefCount() {
    assert(m_refCount>0);
    m_refCount--;
}

int Resource::getRefCount() const {
    return m_refCount;
}

} // namespace slitspace
