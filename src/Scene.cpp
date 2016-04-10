#include <splitspace/Scene.hpp>
#include <splitspace/Engine.hpp>
#include <splitspace/LogManager.hpp>
#include <splitspace/Entity.hpp>
#include <splitspace/Object.hpp>
#include <splitspace/ResourceManager.hpp>

#include <algorithm>
#include <functional>

namespace splitspace {

Scene::Scene(Engine *e, SceneManifest *manifest):
                        Resource(e, manifest),
                        m_rootNode(nullptr),
                        m_resManager(e->resManager),
                        m_engine(e)
{}

bool Scene::load() {
    if(!m_manifest) {
        m_logMan->logErr("(Scene) null scene manifest specified");
        return false;
    }

    if(m_manifest->name.empty()) {
        m_logMan->logErr("(Scene) empty resource names not supppported");
        return false;
    }

    EntityManifest *man = new EntityManifest;
    man->name = m_manifest->name+"_rootNode";
    man->pos = glm::vec3(0);
    man->rot = glm::vec3(0);
    man->scale = glm::vec3(1);
    man->parent = "";
    

    m_rootNode = new Entity(m_engine, man);

    if(!m_rootNode) {
        m_logMan->logErr("(Scene) Out of memory");
        return false;
    }
        

    SceneManifest *sm = static_cast<SceneManifest *>(m_manifest);

    for(auto &it : sm->objects) {
        Object *o = static_cast<Object *>(m_resMan->loadResource(it->name));
        if(!o) {
            continue;
        }
        m_rootNode->addChild(o);
    }

    updateRenderMap();

    m_isLoaded = true;
    return true;
}

void Scene::updateRenderMap() {
    std::function<void (RenderMap&, Entity*)> addObjectRecursive = [&](RenderMap &rm, Entity *e) {
        if(!e) {
            return;
        }
        if(e->getType() == RES_OBJECT) {
            Object *o = static_cast<Object *>(e);
            rm[o->getMaterial()].push_back(o);
        }

        for(auto &it : e->getChildren()) {
            addObjectRecursive(rm, it);
        }
    };

    m_renderMap.clear();
    addObjectRecursive(m_renderMap, m_rootNode);
}

void Scene::unload() {
    m_isLoaded = false;
}

} // namespace splitspace

