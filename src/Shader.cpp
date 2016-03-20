#include <splitspace/Shader.hpp>

namespace splitspace {

Shader::Shader(Engine *e, ShaderManifest *manifest):
                                          Resource(e, manifest),
                                          m_programId(0)
{}

bool Shader::load() {

    return true;
}

void Shader::unload() {

}

}
