#include <splitspace/Shader.hpp>
#include <splitspace/LogManager.hpp>
#include <splitspace/ResourceManager.hpp>
#include <splitspace/Light.hpp>
#include <splitspace/Material.hpp>
#include <splitspace/Texture.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <fstream>

namespace splitspace {

Shader::Shader(Engine *e, ShaderManifest *manifest):
                                          Resource(e, manifest),
                                          m_programId(0),
                                          m_numLights(0),
                                          m_material(nullptr)
{}

bool Shader::load() {
    if(!m_manifest) {
        m_logMan->logErr("(Texture) No manifest specified");
        return false;
    }

    auto loadShader = [this](std::string &src, std::string name) -> bool {
        std::ifstream in(m_resMan->getResPath()+"shaders/"+name);
        if(!in.is_open()) {
            m_logMan->logErr("(Shader) Failed to load "+
                            m_resMan->getResPath()+"shaders/"+name);
            return false;
        }

        src.assign(std::istreambuf_iterator<char>(in),
                   std::istreambuf_iterator<char>());

        in.close();
        return true;
    };


    ShaderManifest *sm = static_cast<ShaderManifest *>(m_manifest);
    std::string vsSrc;
    std::string fsSrc;
    if(!loadShader(vsSrc, sm->vsName)) {
        return false;
    }
    if(!loadShader(fsSrc, sm->fsName)) {
        return false;
    }

    if(!m_renderMan->createShader(vsSrc.c_str(), fsSrc.c_str(), sm->vsVersion,
                                  sm->fsVersion, sm->numOutputs, m_programId)) {
        return false;
    }

    initUniforms(sm->uniformMapping);

    m_isLoaded = true;
    return true;
}

VertexFormat Shader::getInputFormatFromString(const std::string &f) {
    if(f == "VERTEX_3DN") {
        return VERTEX_3DN;
    } else if(f == "VERTEX_3DT") {
        return VERTEX_3DT;
    } else if(f == "VERTEX_3DTN") {
        return VERTEX_3DTN;
    } else {
        return VERTEX_UNKNOWN;
    }
}

UniformType Shader::getUniformTypeFromString(const std::string &u) {
    if(u == "_MVP_") {
        return UNIFORM_MVP_MAT;
    } else if(u == "_TEX_DIFFUSE_") {
        return UNIFORM_TEX_DIFFUSE;
    } else if(u == "_LIGHT_STRUCT_") {
        return UNIFORM_LIGHT_STRUCT;
    } else if(u == "_MATERIAL_STRUCT_") {
        return UNIFORM_MATERIAL_STRUCT;
    } else if(u == "_NUM_LIGHTS_") {
        return UNIFORM_NUM_LIGHTS;
    } else {
        return UNIFORM_UNKNOWN;
    }
}

void Shader::initUniforms(const std::map<std::string, UniformType> &mapping) {
    static const std::string matProps[] = {
        "ambient",
        "diffuse",
        "specular",
        "isTextured",
        "technique"
    };

    static const std::string lightProps[] = {
        "position",
        "rotation",
        "diffuse",
        "specular",
        "spotLightCutoff",
        "power",
        "type"
    };
    for(const auto &u : mapping) {
        switch(u.second) {
            case UNIFORM_MATERIAL_STRUCT: {
                m_materialUniform.name = u.first;
                for(const auto &prop : matProps) {
                    std::string name = u.first+"."+prop;
                    m_materialUniform.locations[prop] = glGetUniformLocation(m_programId, name.c_str());
                }
                break;
            }
            case UNIFORM_LIGHT_STRUCT: {
                m_lightUniform.name = u.first;
                //TODO Eliminate magic numbers and unify MAX_LIGHTS in GLSL and C++
                for(int i = 0;i<8;i++) {
                    std::map<std::string, GLint> mp;
                    for(const auto &prop : lightProps) {
                        std::string name = u.first+"["+std::to_string(i)+"]."+prop;
                        mp[prop] = glGetUniformLocation(m_programId, name.c_str());
                    }
                    m_lightUniform.locations.push_back(mp);
                }
                break;
            }
            default:
                m_genericUniforms[u.second] = GenericUniform{u.first,
                                              glGetUniformLocation(m_programId, u.first.c_str())};
                break;
        }
    }
}

void Shader::setLight(int id, const Light *light) {
    if(!light || id>m_numLights-1) {
        return;
    }
    m_lights[id] = light;
}

void Shader::setMaterial(const Material *mat) {
    m_material = mat;
}

void Shader::setMVP(const glm::mat4 &mvp) {
    setUniform(m_genericUniforms[UNIFORM_MVP_MAT].location, mvp);
}

void Shader::updateMaterialUniform() {
    if(!m_material || m_materialUniform.name.empty()) {
        return;
    }
    MaterialManifest *mm = static_cast<MaterialManifest *>(m_material->getManifest());
    if(!mm) {
        return;
    }
    setUniform(m_materialUniform.locations["ambient"], mm->ambient);
    setUniform(m_materialUniform.locations["diffuse"], mm->diffuse);
    setUniform(m_materialUniform.locations["specular"], mm->specular);
    setUniform(m_materialUniform.locations["technique"], 1);
    Texture *diffuseMap = m_material->getDiffuseMap();
    Texture *normalMap = m_material->getNormalMap();
    if(diffuseMap && m_genericUniforms.find(UNIFORM_TEX_DIFFUSE)!=m_genericUniforms.end()) {
        setUniform(m_materialUniform.locations["isTextured"], true);
        //TODO:
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap->getGLName());
        setUniform(m_genericUniforms[UNIFORM_TEX_DIFFUSE].location, 0);
    } else {
        setUniform(m_materialUniform.locations["isTextured"], false);
    }

    if(normalMap && m_genericUniforms.find(UNIFORM_TEX_NORMAL)!=m_genericUniforms.end()) {
        //TODO:
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, diffuseMap->getGLName());
        setUniform(m_genericUniforms[UNIFORM_TEX_NORMAL].location, 1);
    }
}

void Shader::updateLightUniform() {
    if(m_numLights<=0) {
        return;
    }
    int i = 0;
    for(auto &light : m_lightUniform.locations) {
        LightManifest *lm = static_cast<LightManifest *>(m_lights[i]->getManifest());
        if(!lm) {
            continue;
        }
        setUniform(light["position"], lm->pos);
        setUniform(light["rotation"], lm->rot);
        setUniform(light["diffuse"], lm->diffuse);
        setUniform(light["specular"], lm->specular);
        if(lm->lightType == LIGHT_SPOT) {
            setUniform(light["spotLightCutoff"], lm->spotLightCutoff);
        }
        setUniform(light["power"], lm->power);
        setUniform(light["type"], (int)lm->lightType);

        i++;
    }
}

void Shader::setUniform(GLint id, float val) {
    if(id>=0) {
        glUniform1f(id, val);
    }
}

void Shader::setUniform(GLint id, int val) {
    if(id>=0) {
        glUniform1i(id, val);
    }
}

void Shader::setUniform(GLint id, const glm::vec3 &val) {
    if(id>=0) {
        glUniform3f(id, val.x, val.y, val.z);
    }
}

void Shader::setUniform(GLint id, const glm::vec4 &val) {
    if(id>=0) {
        glUniform4f(id, val.x, val.y, val.z, val.w);
    }
}

void Shader::setUniform(GLint id, const glm::mat4 &val) {
    if(id>=0) {
        glUniformMatrix4fv(id, 1, GL_FALSE, (const GLfloat*)glm::value_ptr(val));
    }
}


void Shader::unload() {
    m_logMan->logInfo("(Shader) Unloading "+m_manifest->name);
    m_renderMan->destroyShader(m_programId);
    m_isLoaded = false;
}

}
