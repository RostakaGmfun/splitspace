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
    UNIFORM_UNKNOWN,
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
    int vsVersion;
    int fsVersion;
    VertexFormat inputFormat;
    int numOutputs;
    std::map<std::string, UniformType> uniformMapping;
};

class Shader: public Resource {
public:
    Shader(Engine *e, ShaderManifest *manifest);
    virtual ~Shader() { unload(); }

    static VertexFormat getInputFormatFromString(const std::string &f);
    static UniformType getUniformTypeFromString(const std::string &u);

    virtual bool load();
    virtual void unload();

private:
    GLuint m_programId;
};

}
#endif // SHADER_HPP
