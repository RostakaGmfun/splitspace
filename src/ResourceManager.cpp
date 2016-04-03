#include <splitspace/ResourceManager.hpp>
#include <splitspace/LogManager.hpp>
#include <splitspace/Engine.hpp>
#include <splitspace/Texture.hpp>
#include <splitspace/Object.hpp>
#include <splitspace/Material.hpp>
#include <splitspace/Mesh.hpp>
#include <splitspace/Scene.hpp>
#include <splitspace/Light.hpp>
#include <splitspace/RenderManager.hpp>
#include <splitspace/Shader.hpp>

#include <algorithm>
#include <fstream>

#include <json/json.hpp>

using json = nlohmann::json;

static bool readVec2(glm::vec2 &vec, json &array) {
    if(array.size() == 2) {
        vec = glm::vec2(float(array[0]), float(array[1]));
        return true;
    }
    return false;
}

static bool readVec3(glm::vec3 &vec, json &array) {
    if(array.size() == 3) {
        vec = glm::vec3(float(array[0]), float(array[1]), float(array[2])); 
        return true;
    }
    return false;
}


namespace splitspace {

ResourceManager::ResourceManager(Engine *e, const std::string &resPath): m_engine(e), 
                                             m_logMan(e->logManager),
                                             m_totalResLoaded(0),
                                             m_totalResFails(0),
                                             m_resPath(resPath)
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
        if(!loadMaterialLib(*it)) {
            return false;
        }
    }

    return true;
}

bool ResourceManager::loadMaterialLib(const std::string &name) {
    if(name.empty()) {
        m_logMan->logErr("(ResourceManager) Empty resource names not supported");
        return false;
    }

    MaterialManifest *mm = nullptr;
    std::string path = m_resPath+"materials/"+name+".json";
    std::ifstream f(path);
    if(!f.is_open()) {
        m_logMan->logErr("(ResourceManager) Error opening "+path);
        return false;
    }
    json jmatlib;
    try {
        jmatlib << f;
        jmatlib = jmatlib["materials"];
    } catch(std::domain_error e) {
        m_logMan->logErr("(ResourceManager) Error parsing "+path);
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
                m_logMan->logErr("(ResourceManager) "+path+":");
                m_logMan->logErr("(ResourceManager) Empty material names not supported");
                delete mm;
                continue;
            }
            mm->name = (*it)["name"];
            if(!readVec3(mm->ambient, (*it)["ambient"])) {
                m_logMan->logWarn("(ResourceManager) at "+path+" in "+mm->name+": ambient should contain 3 elements");
            }

            if(!readVec3(mm->diffuse, (*it)["diffuse"])) {
                m_logMan->logWarn("(ResourceManager) at "+path+" in "+mm->name+": diffuse should contain 3 elements");
            }

            if(!readVec3(mm->specular, (*it)["specular"])) {
                m_logMan->logWarn("(ResourceManager) at "+path+" in "+mm->name+": specular should contain 3 elements");
            }

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
                if((*it)["mapping"]["mipmapping"].is_boolean()) {
                    mm->mipmappingEnabled = (*it)["mapping"]["mipmapping"];
                } else {
                    mm->mipmappingEnabled = false;
                }
                auto jr = (*it)["mapping"]["repeat"];
                if(jr.is_null() || !jr.is_array()) {
                    mm->repeatX = mm->repeatY = 1;
                } else {
                    glm::vec2 vv;
                    if(!readVec2(vv, jr)) {
                        m_logMan->logWarn("(ResourceManager) at "+path+" in "+mm->name+": mapping.repeat should contain 2 elements");
                    }
                    mm->repeatX = vv.x;
                    mm->repeatY = vv.y;
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
                m_logMan->logWarn("(ResourceManager) "+path+":");
                m_logMan->logWarn("(ResourceManager) \"mapping\" is expected to be object");
                delete mm;
            }
        } catch(std::domain_error e) {
            m_logMan->logErr("(ResourceManager): "+path+":");
            m_logMan->logErr("(ResourceManager): "+std::string(e.what()));
            delete mm;
            return false;
        }
        addManifest(mm);
   
    }
    
    return true;
}

bool ResourceManager::loadShaderSupport(const std::string &path) {
    std::ifstream f(path);
    if(!f.is_open()) {
        m_logMan->logErr("(ResourceManager) failed to load shader support from "+path);
        return false;
    }
    m_shaderSupport.assign(std::istreambuf_iterator<char>(f),
                           std::istreambuf_iterator<char>());

    return true;
}

bool ResourceManager::createSceneManifests(const std::vector<std::string> &scenes) {
    for(auto it = scenes.begin();it!=scenes.end();it++) {
        if(!createScene(*it))
            return false;
    }
    return true;
}

bool ResourceManager::loadShaderLib(const std::string &name) {
    std::string path = m_resPath+"shaders/"+name+".json";
    std::ifstream f(path);
    if(!f.is_open()) {
        m_logMan->logErr("(ResourceManager) Failed to load shader library from "+path);
        return false;
    }

    json jshaders;

    try {
        jshaders << f;
        if(jshaders["_DEFAULT_SHADER_"].is_null()) {
            m_logMan->logErr("(ResourceManager) No _DEFAULT_SHADER_ specified in "+path);
            return false;
        }
        m_defaultShader = jshaders["_DEFAULT_SHADER_"];
        jshaders = jshaders["shaders"];
    } catch(std::domain_error e) {
        m_logMan->logErr("ResourceManager) Failed to parse shader library "+path);
        return false;
    }

    for(auto &shader : jshaders) {
        try {
            ShaderManifest *sm = new ShaderManifest;
            if(!sm) {
                m_logMan->logErr("(ResourceManager) Out of memory");
                return false;
            }
            sm->name = shader["name"];
            sm->vsName = shader["vsName"];
            sm->fsName = shader["fsName"];
            sm->vsVersion = shader["vsVersion"];
            sm->fsVersion = shader["fsVersion"];
            sm->inputFormat = Shader::getInputFormatFromString(shader["inputFormat"]);
            sm->numOutputs = shader["numOutputs"];
            for( auto &uniform : shader["uniforms"]) {
                for(json::iterator u = uniform.begin();u!=uniform.end();u++) {
                    sm->uniformMapping[u.value()] = Shader::getUniformTypeFromString(u.key());
                }
            }
            addManifest(sm);
        } catch(std::domain_error e) {
            m_logMan->logErr("(ResourceManager): "+path+":");
            m_logMan->logErr("(ResorceManager): "+std::string(e.what()));
            return false;
        }
    }

    return true;
}

bool ResourceManager::createScene(const std::string &name) {
    SceneManifest *sceneMan = nullptr;

    if(name.empty()) {
        m_logMan->logErr("(ResourceManager) Empty scene names not supported");
        return false;
    }
    std::string path = m_resPath+"/scenes/"+name+".json";
    std::ifstream f(path);
    if(!f.is_open()) {
        m_logMan->logErr("(ResourceManager) Error opening "+path);
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
        m_logMan->logErr("(ResourceManager) "+path+":");
        m_logMan->logErr("\tParse error: "+std::string(e.what()));
        return false;
    }

    auto jobjects = jscene["objects"];
    if(jobjects.is_null() || !jobjects.is_array()) {
        m_logMan->logErr("(ResourceManager) \""+name+"\": \"objects\" array expected");
        return false;
    }

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
                if(!readVec3(objMan->pos, jo["transform"]["position"])) {
                    m_logMan->logWarn("(ResourceManager) at "+path+" in "+objMan->name+": transform.position should contain 3 elements");
                }

                if(!readVec3(objMan->rot, jo["transform"]["rotation"])) {
                    m_logMan->logWarn("(ResourceManager) at "+path+" in "+objMan->name+": transform.rotation should contain 3 elements");
                }

                if(!readVec3(objMan->scale, jo["transform"]["scaling"])) {
                    m_logMan->logWarn("(ResourceManager) at "+path+" in "+objMan->name+": transform.rotation should contain 3 elements");
                }
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
        sceneMan->objects.push_back(objMan);
        addManifest(objMan);
    }

    auto jlights = jscene["lights"];
    if(jlights.is_null()) {
        addManifest(sceneMan);
        m_logMan->logInfo("(ResourceManager) Created manifest for Scene \""+name+"\"");
        return true;
    }
    if(!jlights.is_array()) {
        m_logMan->logErr("(ResourceManager) \""+name+"\": \"lights\" array expected");
        return false;
    }

    LightManifest *lightMan = nullptr;
    for(auto it = jlights.begin();it!=jlights.end();it++) {
        auto jo = (*it);
        if(jo["name"].is_null() || !jo["name"].is_string()) {
            m_logMan->logErr("(ResourceManager) \""+name+"\": expected lights name");
            delete sceneMan;
            return false;
        }
        std::string lightName = jo["name"];
        lightMan = new LightManifest();
        if(!objMan) {
            m_logMan->logErr("(ResourceManager) Out of memory");
            delete sceneMan;
            return false;
        }
        lightMan->name = lightName;
        try {
            std::string lightType = jo["type"];
            lightMan->lightType = Light::getTypeFromName(lightType);
            auto jtransform = jo["transform"];
            if(!jtransform.is_null()) {
                readVec3(lightMan->pos, jtransform["position"]);
                readVec3(lightMan->rot, jtransform["rotation"]);
            }
            readVec3(lightMan->diffuse, jo["diffuse"]);
            readVec3(lightMan->specular, jo["specular"]);
            lightMan->power = jo["power"].is_null()?1.f:float(jo["power"]);
        } catch(std::domain_error e) {
            m_logMan->logErr("(ResourceManager) \""+lightName+"\":");
            m_logMan->logErr("\tParse error: "+std::string(e.what()));
            if(objMan)
                delete objMan;
        }
        sceneMan->lights.push_back(lightMan);
        addManifest(lightMan);
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
        m_totalResFails++;
        return nullptr;
    }
    
    if(m_resourceCache.find(name)==m_resourceCache.end()) {

        auto it = m_resourceManifests.find(name);
        if(it == m_resourceManifests.end()) {
            m_logMan->logErr("(ResourceManager) No Resource with name \""+name+"\" found");
            m_totalResFails++;
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
            case RES_ENTITY: {
                EntityManifest *m = static_cast<EntityManifest *>(it->second);
                res = new Entity(m_engine, m);
            break; }
            case RES_LIGHT: {
                LightManifest *m = static_cast<LightManifest *>(it->second);
                res = new Light(m_engine, m);
            break; }
            case RES_SHADER: {
                ShaderManifest *m = static_cast<ShaderManifest *>(it->second);
                res = new Shader(m_engine, m);
            break; }
            default:
                m_logMan->logErr("(ResourceManager) Unknown or unsupported Resource");
                m_totalResFails++;
                return nullptr;
        }

        if(!res) {
            m_logMan->logErr("(ResourceManager) Out of memory");
            m_totalResFails++;
            return nullptr;
        }
        
        res->incRefCount();
        if(!res->load()) {
            m_logMan->logErr("(ResourceManager) Error loading \""+name+"\"");
            delete res;
            m_totalResFails++;
            return nullptr;
        }

        m_resourceCache[name] = res;
        m_totalResLoaded++;
    }

    Resource *r = m_resourceCache[name];
    return r;
}

void ResourceManager::destroy() {

}

bool ResourceManager::unloadResource(const std::string &name) {
    if(name.empty()) {
        m_logMan->logErr("(ResourceManager) Empty resource names not supported");
        return false;
    }
    auto it = m_resourceCache.find(name);

    if(it == m_resourceCache.end()) {
        auto itman = m_resourceManifests.find(name);
        if(itman == m_resourceManifests.end()) {
            m_logMan->logErr("(ResourceManager) Resource "
                            +name+" does not exits. Cannot unload it");
            return false;
        } else {
            m_logMan->logWarn("ResourceManager) Resource "
                              +name+" is not loaded so it can't be unloaded");
            return false;
        }
    }
    
    it->second->unload();
    it->second->decRefCount();
    return true;
}

int ResourceManager::collectGarbage() {
    int numGarbageCoollected = 0;

    for( auto res : m_resourceCache ) {
        if(res.second->getRefCount() == 0) {
            res.second->unload();
            numGarbageCoollected++;
        }
    }

    return numGarbageCoollected;
}

void ResourceManager::logStats() {
    m_logMan->logInfo("(ResourceManager) STATS:");
    m_logMan->logInfo("\t Total resources created: "+std::to_string(m_totalResLoaded));
    m_logMan->logInfo("\t Total failed resource loading: "+std::to_string(m_totalResFails));
}

} // namespace splitspace
