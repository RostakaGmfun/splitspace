#include <splitspace/ResourceManager.hpp>
#include <splitspace/LogManager.hpp>
#include <splitspace/Engine.hpp>
#include <splitspace/Texture.hpp>
#include <splitspace/Object.hpp>
#include <splitspace/Material.hpp>
#include <splitspace/Mesh.hpp>
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
    
TextureManifest *ResourceManager::readTextureManifest(std::string name) {

    TextureManifest *tm = getTextureManifest(name);
    if(!tm) {
        m_logMan->logInfo("(ResourceManager) Creating texture manifest \""
                          +name+"\"");
        tm = new TextureManifest;
        if(!tm) {
            m_logMan->logErr("(ResourceManager) Out of memory");
            return nullptr;
        }
        tm->name = name;
        addTextureManifest(tm);
    }
    return tm;
}
    
bool ResourceManager::loadMaterialLib(std::vector<std::string> ml) {
    for(auto it = ml.begin();it!=ml.end();it++) {
        loadMaterialLib(*it);
    }

    return true;
}

bool ResourceManager::loadMaterialLib(std::string name) {
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
        addMaterialManifest(mm);
   
    }
    
    return true;
}

bool ResourceManager::addTextureManifest(TextureManifest *tm) {
    if(!tm)
        return false;

    if(tm->name.empty()) {
        m_logMan->logErr("(ResourceManager) empty resource names not supported");
        return false;
    }

    if(m_textureManifests.find(tm->name)!=m_textureManifests.end()) {
        m_logMan->logErr("(ResourceManager) Texture with name \""
                                    +tm->name+"\" already exists");
        return false;
    }

    m_textureManifests[tm->name] = tm;

    return true;
}

bool ResourceManager::addObjectManifest(ObjectManifest *om) {
    if(!om)
        return false;

    if(om->name.empty()) {
        m_logMan->logErr("(ResourceManager) empty resource names not supported");
        return false;
    }

    if(m_objectManifests.find(om->name)!=m_objectManifests.end()) {
        m_logMan->logErr("(ResourceManager) Object with name \""
                                    +om->name+"\" already exists");
        return false;
    }

    m_objectManifests[om->name] = om;

    return true;
}

bool ResourceManager::addMeshManifest(MeshManifest *mm) {
    if(!mm)
        return false;

    if(mm->name.empty()) {
        m_logMan->logErr("(ResourceManager) empty resource names not supported");
        return false;
    }

    if(m_meshManifests.find(mm->name)!=m_meshManifests.end()) {
        m_logMan->logErr("(ResourceManager) Mesh with name \""
                                    +mm->name+"\" already exists");
        return false;
    }

    m_meshManifests[mm->name] = mm;

    return true;
}

bool ResourceManager::addMaterialManifest(MaterialManifest *mm) {
    if(!mm)
        return false;

    if(mm->name.empty()) {
        m_logMan->logErr("(ResourceManager) empty resource names not supported");
        return false;
    }

    if(m_materialManifests.find(mm->name)!=m_materialManifests.end()) {
        m_logMan->logErr("(ResourceManager) Material with name \""
                                    +mm->name+"\" already exists");
        return false;
    }

    m_materialManifests[mm->name] = mm;

    return true;
}
    
TextureManifest *ResourceManager::getTextureManifest(std::string name) {
    auto it = m_textureManifests.find(name);
    if(it == m_textureManifests.end())
        return nullptr;
    return it->second;
}

ObjectManifest *ResourceManager::getObjectManifest(std::string name) {
    auto it = m_objectManifests.find(name);
    if(it == m_objectManifests.end())
        return nullptr;
    return it->second;

}

MeshManifest *ResourceManager::getMeshManifest(std::string name) {
    auto it = m_meshManifests.find(name);
    if(it == m_meshManifests.end())
        return nullptr;
    return it->second;

}

MaterialManifest *ResourceManager::getMaterialManifest(std::string name) {
    auto it = m_materialManifests.find(name);
    if(it == m_materialManifests.end())
        return nullptr;
    return it->second;
}

ShaderManifest *ResourceManager::getShaderManifest(std::string name) {
    auto it = m_shaderManifests.find(name);
    if(it == m_shaderManifests.end())
        return nullptr;
    return it->second;

}

Texture *ResourceManager::loadTexture(std::string name) {
    if(name.empty()) {
        m_logMan->logErr("(ResourceManager) Emptu resource names not supported");
        return nullptr;
    }

    if(m_textureCache.find(name)==m_textureCache.end()) {

        auto it = m_textureManifests.find(name);
        if(it == m_textureManifests.end()) {
            m_logMan->logErr("(ResourceManager) No Texture with name \""+name+"\" found");
            return nullptr;
        }

        m_logMan->logInfo("(ResourceManager) Loading Texture \""+name+"\"");

        Texture *tex = new Texture(m_engine, (it->second));
        if(!tex) {
            m_logMan->logErr("(ResourceManager) Out of memory");
            return nullptr;
        }
        
        if(!tex->load()) {
            m_logMan->logErr("(ResourceManager) Error loading \""+name+"\"");
            delete tex;
            return nullptr;
        }

        m_textureCache[name] = tex;
    }

    Texture *t = m_textureCache[name];
    t->incRefCount();
    return t;
}

Material *ResourceManager::loadMaterial(std::string name) {
    if(name.empty()) {
        m_logMan->logErr("(ResourceManager) Empty resource names not supported");
        return nullptr;
    }

    if(m_materialManifests.find(name) == m_materialManifests.end()) {
        m_logMan->logErr("(ResourceManager) Material \""+name+"\" does not exist");
        return nullptr;
    }
    
    auto it = m_materialCache.find(name);

    if(it == m_materialCache.end()) {
        m_logMan->logInfo("(ResourceManager) Loading Material \""+name+"\"");
        Material *m = new Material(m_engine, m_materialManifests[name]);
        if(!m) {
            m_logMan->logErr("(ResourceManager) Out of memory");
            return nullptr;
        }

        if(!m->load()) {
            m_logMan->logErr("(ResourceManager) Failed to load Material \""+name+"\"");
            return nullptr;
        }

        m_materialCache[name] = m;
    }
    
    Material *m = m_materialCache[name];
    m->incRefCount();
    return m;

}

void ResourceManager::destroy() {

}

} // namespace splitspace
