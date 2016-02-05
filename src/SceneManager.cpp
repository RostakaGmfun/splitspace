#include <splitspace/SceneManager.hpp>
#include <splitspace/RenderManager.hpp>
#include <splitspace/ResourceManager.hpp>
#include <splitspace/Scene.hpp>
#include <splitspace/Engine.hpp>
#include <splitspace/LogManager.hpp>
#include <splitspace/Object.hpp>
#include <splitspace/Mesh.hpp>
#include <splitspace/Material.hpp>

#include <json/json.hpp>

using json = nlohmann::json;

#include <fstream>

static void readVec3(glm::vec3 &vec, json &array) {
    if(array.size() == 1) {
        vec = glm::vec3(float(array[0]));
    } else if(array.size()<3) {
        vec = glm::vec3(0); 
    } else {
        vec = glm::vec3(float(array[0]), float(array[1]), float(array[2])); 
    }
}

namespace splitspace {
SceneManager::SceneManager(Engine *e): m_currentScene(nullptr),
                                       m_engine(e),
                                       m_logMan(e->logManager),
                                       m_resMan(e->resManager) {
    m_scenes.clear();
}
SceneManager::~SceneManager() {
    
}

bool SceneManager::init(std::vector<std::string> scenes) {

    if(!m_scenes.empty()) {
        for(auto it = m_scenes.begin();it!=m_scenes.end();it++) {
            it->second->unload();
            delete it->second;
        }
    }

    for(auto it = scenes.begin();it!=scenes.end();it++) {
        if(!sceneCreate(*it))
            return false;
    }

    return true;
}

bool SceneManager::sceneCreate(std::string name) {
    SceneManifest *sceneMan = nullptr;

    if(name.empty()) {
        m_logMan->logErr("(SceneManager) Empty scene names not supported");
        return false;
    }
    
    std::ifstream f("data/scenes/"+name+".json");
    if(!f.is_open()) {
        m_logMan->logErr("(SceneManager) Error opening data/scenes/"+name+".json");
        return false;
    }
    
    sceneMan = new SceneManifest;
    if(!sceneMan) {
        m_logMan->logErr("(SceneManager) Out of memory");
        return false;
    }

    sceneMan->name = name;

    json jscene; 
    try {
        jscene << f;
    } catch(std::domain_error e) {
        m_logMan->logErr("(SceneManager) data/scenes/"+name+".json");
        m_logMan->logErr("\tParse error: "+std::string(e.what()));
        return false;
    }

    auto jobjects = jscene["objects"];
    if(jobjects.is_null() || !jobjects.is_array()) {
        m_logMan->logErr("(SceneManager) \""+name+"\": \"objects\" array expected");
        return false;
    }

    int numObjects = 0;
    ObjectManifest *objMan = nullptr;
    for(auto it = jobjects.begin();it!=jobjects.end();it++) {
        auto jo = (*it);
        if(jo["name"].is_null() || !jo["name"].is_string()) {
            m_logMan->logErr("(SceneManager) \""+name+"\": expected object name");
            return false;
        }
        std::string objectName = jo["name"];
        objMan = new ObjectManifest();
        if(!objMan) {
            m_logMan->logErr("(SceneManager) Out of memory");
            return false;
        }
        objMan->meshManifest = new MeshManifest();
        try {
            objMan->meshManifest->name = jo["mesh"];
            if(jo["material"].is_null()) {
                objMan->materialManifest = nullptr;
            } else {
                std::string matName = jo["material"];
                objMan->materialManifest = m_resMan->getMaterialManifest(matName);
                if(!objMan->materialManifest) {
                    m_logMan->logWarn("(SceneManager) Material \""+matName+"\" not found");
                    // TODO
                }
            }
            if(jo["transform"].is_null()) {
                objMan->scale = glm::vec3(1);
            } else {
                readVec3(objMan->pos, jo["transform"]["position"]);
                readVec3(objMan->rot, jo["transform"]["rotation"]);
                readVec3(objMan->scale, jo["transform"]["scaling"]);
            }
        } catch(std::domain_error e) {
            m_logMan->logErr("(SceneManager) \""+objectName+"\":");
            m_logMan->logErr("\tParse error: "+std::string(e.what()));
            if(objMan)
                delete objMan;
        }
        numObjects++;
        sceneMan->objects.push_back(objMan);
    }

    Scene *scene = new Scene(m_engine, sceneMan);
    if(!scene) {
        m_logMan->logErr("(SceneManager) Out of memory");
        return false;
    }

    m_scenes[name] = scene;

    m_logMan->logInfo("(SceneManager) Created manifest for Scene \""+name+"\"");

    return true;
}

void SceneManager::update(float dt) {

}

bool SceneManager::loadScene(std::string name) {
    if(name.empty()) {
        m_logMan->logErr("(SceneManager) Empty scenes not suported");
        return false;
    }
    auto it = m_scenes.find(name);
    if(it == m_scenes.end()) {
        m_logMan->logErr("(SceneManager) Scene \""+name+"\" does not exist");
        return false;
    }

    if(m_currentScene) {
        m_currentScene->unload();
    }

    m_logMan->logInfo("(SceneManager) Loading Scene \""+name+"\"");

    if(!it->second->load()) {
        m_logMan->logErr("(SceneManager) Failed to load \""+name+"\"");
        return false;
    }
    
    m_currentScene = it->second;
    return true;
}

} // namespace splitspace
