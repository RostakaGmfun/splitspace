#ifndef SHADER_HPP
#define SHADER_HPP

#include <splitspace/Resource.hpp>
#include <splitspace/RenderManager.hpp>

#include <vector>
#include <map>

namespace splitspace {

enum ShaderUsage {
    SHADER_USAGE_OBJECT,
    SHADER_USAGE_RTV
};

enum UniformType {
    UNIFORM_VIEW_MAT,
    UNIFORM_PROJ_MAT,
    UNIFORM_MODEL_MAT,
    UNIFORM_MVP_MAT,

    UNIFORM_TEX_DIFFUSE,
    UNIFORM_TEX_NORMAL,

    UNIFORM_LIGHT_STRUCT,
    UNIFORM_SHADING_TYPE
};

struct ShaderManifest: public ResourceManifest {
    ShaderManifest(): ResourceManifest(RES_SHADER)
    {}
    std::string vsName;
    std::string fsName;
    GLSLVersion vsVersion;
    GLSLVersion fsVersion;
    VertexFormat inputFormat;
    int numOutputs;
    std::map<UniformType, std::string> uniformMapping;
};

class Shader: public Resource {
public:
    Shader(Engine *e, ShaderManifest *manifest);
    virtual ~Shader() { unload(); }

    virtual bool load();
    virtual void unload();

private:
    GLuint m_programId;
};

}
#endif // SHADER_HPP
