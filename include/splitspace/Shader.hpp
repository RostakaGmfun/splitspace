#ifndef SHADER_HPP
#define SHADER_HPP

#include <splitspace/Resource.hpp>
#include <splitspace/RenderManager.hpp>
#include <splitspace/Scene.hpp>

#include <vector>
#include <map>

#include <glm/glm.hpp>

namespace splitspace {

class Light;
class Material;

enum ShaderUsage {
    SHADER_USAGE_OBJECT,
    SHADER_USAGE_RTV
};

enum UniformType {
    UNIFORM_UNKNOWN,
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

    void setNumLights(int num);
    void setLight(int lightId, const Light *l);
    void clearLights();
    void setMaterial(const Material *mat);

    void setMVP(const glm::mat4 &mvp);

    void updateMaterialUniform();

    GLuint getProgramId() const { return m_programId; }

private:
    void initUniforms(const std::map<std::string, UniformType> &mapping);
    void setUniform(GLint id, float val);
    void setUniform(GLint id, int val);
    void setUniform(GLint id, const glm::vec3 &val);
    void setUniform(GLint id, const glm::vec4 &val);
    void setUniform(GLint id, const glm::mat4 &val);


private:
    GLuint m_programId;

    struct LightUniform {
        std::string name;
        std::vector<std::map<std::string, GLint>> locations;
    };

    LightUniform m_lightUniform;

    struct MaterialUniform {
        std::string name;
        std::map<std::string, GLint> locations;
    };

    MaterialUniform m_materialUniform;

    struct GenericUniform {
        std::string name;
        GLint location;
    };
    std::map<UniformType, GenericUniform> m_genericUniforms;
};

}
#endif // SHADER_HPP
