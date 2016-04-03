#include <splitspace/Scene.hpp>
#include <splitspace/Engine.hpp>
#include <splitspace/LogManager.hpp>
#include <splitspace/Entity.hpp>
#include <splitspace/Object.hpp>
#include <splitspace/ResourceManager.hpp>

#include <algorithm>

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

    for(auto it = sm->objects.begin();it!=sm->objects.end();it++) {
        Object *o = static_cast<Object *>(m_resMan->loadResource((*it)->name));
        if(!o) {
            continue;
        }
        if((*it)->name.empty()) {
            m_rootNode->addChild(o);
        } else {
            Entity *e = static_cast<Object *>(m_resMan->loadResource((*it)->name));
            if(e) {
                e->addChild(o);
            } else {
                m_rootNode->addChild(o);
            }
        }

        auto objListItr = m_renderMap.find(o->getMaterial());
        if(objListItr == m_renderMap.end()) {
            m_renderMap[o->getMaterial()].push_back(o);
        } else {
            auto &objL = m_renderMap[o->getMaterial()];
            auto objItr = std::find(objL.begin(), objL.end(), o);
            if(objItr == objL.end()) {
                objL.push_back(o);
            }
        }
        
    }

    m_isLoaded = true;
    return true;
}

void Scene::unload() {
    m_isLoaded = false;
}

} // namespace splitspace

