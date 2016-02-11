#include <splitspace/ResourceManager.hpp>
#include <splitspace/LogManager.hpp>
#include <splitspace/Engine.hpp>
#include <splitspace/Texture.hpp>
#include <splitspace/Object.hpp>
#include <splitspace/Material.hpp>
#include <splitspace/Mesh.hpp>
#include <splitspace/Scene.hpp>
#include <splitspace/RenderManager.hpp>

#include <algorithm>
#include <fstream>

#include <json/json.hpp>

using json = nlohmann::json;

static void readVec2(glm::vec2 &vec, json &array) {
    if(array.size() == 1) {
        vec = glm::vec2(float(array[0]));
    } else {
        vec = glm::vec2(float(array[0]), float(array[1]));
    }
}

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

ResourceManager::ResourceManager(Engine *e): m_engine(e), 
                                             m_logMan(e->logManager)
{}
    
ResourceManager::~ResourceManager() {
    destroy();
}
    
TextureManifest *ResourceManager::readTextureManifest(const std::string &name) {

    TextureManifest *tm = static_cast<TextureManifest *>(getManifest(name));
    if(!tm) {
        tm = new TextureManifest;
        if(!tm) {
            m_logMan->logErr("(ResourceManager) Out of memory");
            return nullptr;
        }
        tm->name = name;
        addManifest(tm);
    }
    return tm;
}
    
bool ResourceManager::loadMaterialLib(const std::vector<std::string> &ml) {
    for(auto it = ml.begin();it!=ml.end();it++) {
        loadMaterialLib(*it);
    }

    return true;
}

bool ResourceManager::loadMaterialLib(const std::string &name) {
    MaterialManifest *mm = nullptr;
    std::ifstream f("data/materials/"+name+".json");
    if(!f.is_open()) {
        m_logMan->logErr("(ResourceManager) Error opening data/materials/"+name+".json");
        return false;
    }
    json jmatlib;
    try {
        jmatlib << f;
        jmatlib = jmatlib["materials"];
    } catch(std::domain_error e) {
        m_logMan->logErr("(ResourceManager) Error parsing data/materials/"+name+".json");
        m_logMan->logErr("(ResourceManager)\t"+std::string(e.what()));
        return false;
    }
    for(auto it = jmatlib.begin();it!=jmatlib.end();it++) {
        mm = new MaterialManifest;
        if(!mm) {
            m_logMan->logErr("(ResourceManager) Out of memory");
            return false;
        }
        
        try {
            if((*it)["name"].is_null()) {
                m_logMan->logErr("(ResourceManager) data/materials/"+name+".json:");
                m_logMan->logErr("(ResourceManager) Empty material names not supported");
                delete mm;
                continue;
            }
            mm->name = (*it)["name"];
            readVec3(mm->ambient, (*it)["ambient"]);
            readVec3(mm->diffuse, (*it)["diffuse"]);
            readVec3(mm->specular, (*it)["specular"]);
            if((*it)["diffuseMap"].is_null()) {
                mm->diffuseMap = nullptr;
            } else {
                std::string df = (*it)["diffuseMap"];
                mm->diffuseMap = readTextureManifest(df);
            }
            
            if((*it)["normalMap"].is_null()) {
                mm->normalMap = nullptr;
            } else {
                std::string nm = (*it)["normalMap"];
                mm->normalMap = readTextureManifest(nm);
            }
            if((*it)["mapping"].is_null()) {
                mm->mipmappingEnabled = false;
                mm->repeatX = mm->repeatY = 1;
                mm->filtering = TEX_FILTER_NEAREST;
            } else if((*it)["mapping"].is_object()) {
                mm->mipmappingEnabled = (*it)["mapping"]["mipampping"];
                auto jr = (*it)["mapping"]["repeat"];
                if(jr.is_null() || !jr.is_array()) {
                    mm->repeatX = mm->repeatY = 1;
                } else {
                    glm::vec2 vv;
                    readVec2(vv, jr);
                }
                if((*it)["mapping"]["filtering"].is_null()) {
                    mm->filtering = TEX_FILTER_NEAREST;
                } else {
                    std::string flt = (*it)["mapping"]["filtering"];
                    if(flt == "linear") {
                        mm->filtering = TEX_FILTER_LINEAR;
                    } else if(flt == "nearest") {
                        mm->filtering = TEX_FILTER_NEAREST;
                    } else {
                        m_logMan->logWarn("(ResourceManager) \""
                        +mm->name+"\" unknown value of \"filtering\" property");
                        mm->filtering = TEX_FILTER_NEAREST;
                    }
                }
            } else {
                m_logMan->logWarn("(ResourceManager) data/materials/"+name+".json:");
                m_logMan->logWarn("(ResourceManager) \"mapping\" is expected to be object");
                delete mm;
            }
        } catch(std::domain_error e) {
            m_logMan->logErr("(ResourceManager): data/materials/"+name+".json:");
            m_logMan->logErr("(ResourceManager): "+std::string(e.what()));
            delete mm;
            return false;
        }
        addManifest(mm);
   
    }
    
    return true;
}

bool ResourceManager::createSceneManifests(const std::vector<std::string> &scenes) {
    for(auto it = scenes.begin();it!=scenes.end();it++) {
        if(!createScene(*it))
            return false;
    }
    return true;
}

bool ResourceManager::createScene(const std::string &name) {
    SceneManifest *sceneMan = nullptr;

    if(name.empty()) {
        m_logMan->logErr("(ResourceManager) Empty scene names not supported");
        return false;
    }
    
    std::ifstream f("data/scenes/"+name+".json");
    if(!f.is_open()) {
        m_logMan->logErr("(ResourceManager) Error opening data/scenes/"+name+".json");
        return false;
    }
    
    sceneMan = new SceneManifest;
    if(!sceneMan) {
        m_logMan->logErr("(ResourceManager) Out of memory");
        return false;
    }

    sceneMan->name = name;

    json jscene; 
    try {
        jscene << f;
    } catch(std::domain_error e) {
        m_logMan->logErr("(ResourceManager) data/scenes/"+name+".json");
        m_logMan->logErr("\tParse error: "+std::string(e.what()));
        return false;
    }

    auto jobjects = jscene["objects"];
    if(jobjects.is_null() || !jobjects.is_array()) {
        m_logMan->logErr("(ResourceManager) \""+name+"\": \"objects\" array expected");
        return false;
    }

    int numObjects = 0;
    ObjectManifest *objMan = nullptr;
    for(auto it = jobjects.begin();it!=jobjects.end();it++) {
        auto jo = (*it);
        if(jo["name"].is_null() || !jo["name"].is_string()) {
            m_logMan->logErr("(ResourceManager) \""+name+"\": expected object name");
            delete sceneMan;
            return false;
        }
        std::string objectName = jo["name"];
        objMan = new ObjectManifest();
        if(!objMan) {
            m_logMan->logErr("(ResourceManager) Out of memory");
            delete sceneMan;
            return false;
        }
        objMan->name = objectName;
        try {
            objMan->meshManifest = static_cast<MeshManifest *>(getManifest(jo["mesh"]));
            if(!objMan->meshManifest) {
                objMan->meshManifest = new MeshManifest;
                if(!objMan->meshManifest) {
                    m_logMan->logErr("(ResourceManager) Out of Memory");
                    delete objMan;
                    delete sceneMan;
                }
                objMan->meshManifest->name = jo["mesh"];
                addManifest(objMan->meshManifest);
            }
            if(jo["material"].is_null()) {
                objMan->materialManifest = nullptr;
                objMan->meshManifest->loadMaterial = true;
            } else {
                std::string matName = jo["material"];
                objMan->materialManifest = static_cast<MaterialManifest *>(getManifest(matName));
                if(!objMan->materialManifest) {
                    m_logMan->logWarn("(ResourceManager) Material \""+matName+"\" not found");
                    // TODO: set default material
                }
            }
            if(jo["transform"].is_null()) {
                objMan->scale = glm::vec3(1);
            } else {
                readVec3(objMan->pos, jo["transform"]["position"]);
                readVec3(objMan->rot, jo["transform"]["rotation"]);
                readVec3(objMan->scale, jo["transform"]["scaling"]);
            }
            if(!jo["parent"].is_null()) {
                objMan->parent = jo["parent"];
            }
        } catch(std::domain_error e) {
            m_logMan->logErr("(ResourceManager) \""+objectName+"\":");
            m_logMan->logErr("\tParse error: "+std::string(e.what()));
            if(objMan)
                delete objMan;
        }
        numObjects++;
        sceneMan->objects.push_back(objMan);
        addManifest(objMan);
    }
    addManifest(sceneMan);
    m_logMan->logInfo("(ResourceManager) Created manifest for Scene \""+name+"\"");

    return true;
}


bool ResourceManager::addManifest(ResourceManifest *rm) {
    if(!rm)
        return false;
    if(rm->name.empty()) {
        m_logMan->logErr("(ResourceManager) Empty resorce names not supported");
        return false;
    }

    if(m_resourceManifests.find(rm->name)!=m_resourceManifests.end()) {
        m_logMan->logErr("(ResourceManager) Resource with name \""
                                            +rm->name+"\" already exists");
        return false;
    }
    
    m_resourceManifests[rm->name] = rm;

    return true;
}

ResourceManifest *ResourceManager::getManifest(const std::string &name) {
    if(name.empty()) {
        m_logMan->logErr("(ResourceManager) Empty resource names not supported");
        return nullptr;
    }
    auto it = m_resourceManifests.find(name);
    if(it == m_resourceManifests.end())
        return nullptr;
    return it->second;
}
    
Resource *ResourceManager::loadResource(const std::string &name) {
    if(name.empty()) {
        m_logMan->logErr("(ResourceManager) Empty resource names not supported");
        return nullptr;
    }
    
    if(m_resourceCache.find(name)==m_resourceCache.end()) {

        auto it = m_resourceManifests.find(name);
        if(it == m_resourceManifests.end()) {
            m_logMan->logErr("(ResourceManager) No Resource with name \""+name+"\" found");
            return nullptr;
        }

        m_logMan->logInfo("(ResourceManager) Loading Resource \""+name+"\"");

        Resource *res;

        switch(it->second->type) {
            case RES_TEXTURE: {
                TextureManifest *m = static_cast<TextureManifest *>(it->second);
                res = new Texture(m_engine, m);
            break; }
            case RES_MATERIAL: {
                MaterialManifest *m = static_cast<MaterialManifest *>(it->second);
                res = new Material(m_engine, m);
            break; }
            case RES_MESH: {
                MeshManifest *m = static_cast<MeshManifest *>(it->second);
                res = new Mesh(m_engine, m);
            break; }
            case RES_OBJECT: {
                ObjectManifest *m = static_cast<ObjectManifest *>(it->second);
                res = new Object(m_engine, m);
            break; }
            case RES_SCENE: {
                SceneManifest *m = static_cast<SceneManifest *>(it->second);
                res = new Scene(m_engine, m);
            break; }
            default:
                m_logMan->logErr("(ResourceManager) Unknown or unsupported Resource");
                return nullptr;
        }

        if(!res) {
            m_logMan->logErr("(ResourceManager) Out of memory");
            return nullptr;
        }
        
        res->incRefCount();
        if(!res->load()) {
            m_logMan->logErr("(ResourceManager) Error loading \""+name+"\"");
            delete res;
            return nullptr;
        }

        m_resourceCache[name] = res;
    }

    Resource *r = m_resourceCache[name];
    return r;

}

void ResourceManager::destroy() {

}

} // namespace splitspace
