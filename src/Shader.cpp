#include <splitspace/Shader.hpp>
#include <splitspace/LogManager.hpp>
#include <splitspace/ResourceManager.hpp>

#include <fstream>

namespace splitspace {

Shader::Shader(Engine *e, ShaderManifest *manifest):
                                          Resource(e, manifest),
                                          m_programId(0)
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

void Shader::unload() {
    m_logMan->logInfo("(Shader) Unloading "+m_manifest->name);
    m_renderMan->destroyShader(m_programId);
    m_isLoaded = false;
}

}
