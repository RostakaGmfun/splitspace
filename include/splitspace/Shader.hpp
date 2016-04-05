#ifndef SHADER_HPP
#define SHADER_HPP

#include <splitspace/Resource.hpp>
#include <splitspace/RenderManager.hpp>

#include <vector>
#include <map>

#include <glm/glm.hpp>

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
    UNIFORM_NUM_LIGHTS,

    UNIFORM_MATERIAL_STRUCT
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

    static VertexFormat getInputFormatFromString(const std::string &f);
    static UniformType getUniformTypeFromString(const std::string &u);

    virtual bool load();
    virtual void unload();

private:
    void initUniforms(const std::map<std::string, UniformType> &mapping);

private:
    GLuint m_programId;
    struct Uniform {
        GLuint id;
        UniformType type;
    };

    std::map<std::string, Uniform> m_uniforms;
};

}
#endif // SHADER_HPP
