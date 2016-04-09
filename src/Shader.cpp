#include <splitspace/Shader.hpp>
#include <splitspace/LogManager.hpp>
#include <splitspace/ResourceManager.hpp>
#include <splitspace/Light.hpp>
#include <splitspace/Material.hpp>

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

    if(!m_renderMan->createShader(vsSrc.c_str(), fsSrc.c_str(), sm->vsVersion, sm->fsVersion,
                                sm->inputFormat, sm->numOutputs, m_programId)) {
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
                m_lightUniform.locations.reserve(8);
                for(int i = 0;i<8;i++) {
                    for(const auto &prop : lightProps) {
                        std::string name = u.first+"["+std::to_string(i)+"]."+prop;
                        m_lightUniform.locations[i][prop] = glGetUniformLocation(m_programId, name.c_str());
                    }
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

bool Shader::setLight(int id, const Light *light) {
    if(!light || id>m_numLights-1) {
        return false;
    }
    m_lights[id] = light;
    return true;
}

bool Shader::setMaterial(const Material *mat) {
    m_material = mat;
    return true;
}

void Shader::updateUniformData() {
    updateMaterialUniform();
    updateLightUniform();

    //TODO: MVP and textures
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
    setUniform(m_materialUniform.locations["isTextured"], mm->diffuseMap == nullptr);
    //TODO:
    setUniform(m_materialUniform.locations["technique"], 1);
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
    glUniform1f(id, val);
}

void Shader::setUniform(GLint id, int val) {
    glUniform1i(id, val);
}

void Shader::setUniform(GLint id, glm::vec3 val) {
    glUniform3f(id, val.x, val.y, val.z);
}

void Shader::setUniform(GLint id, glm::vec4 val) {
    glUniform4f(id, val.x, val.y, val.z, val.w);
}

void Shader::setUniform(GLint id, glm::mat4 val) {
    glUniformMatrix4fv(id, 1, GL_FALSE, (const GLfloat*)glm::value_ptr(val));
}


void Shader::unload() {
    m_logMan->logInfo("(Shader) Unloading "+m_manifest->name);
    m_renderMan->destroyShader(m_programId);
    m_isLoaded = false;
}

}
