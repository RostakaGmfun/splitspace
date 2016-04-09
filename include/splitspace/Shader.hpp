#ifndef SHADER_HPP
#define SHADER_HPP

#include <splitspace/Resource.hpp>
#include <splitspace/RenderManager.hpp>

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

    void setNumLights(int num) { m_numLights = num; }
    int getNumLights() const { return m_numLights; }
    bool setLight(int id, const Light *light);
    bool setMaterial(const Material *mat);


    void updateUniformData();

private:
    void initUniforms(const std::map<std::string, UniformType> &mapping);
    void setUniform(GLint id, float val);
    void setUniform(GLint id, int val);
    void setUniform(GLint id, glm::vec3 val);
    void setUniform(GLint id, glm::vec4 val);
    void setUniform(GLint id, glm::mat4 val);


    void updateMaterialUniform();
    void updateLightUniform();
private:
    GLuint m_programId;

    struct LightUniform {
        std::string name;
        std::vector<std::map<std::string, GLint>> locations;
    };

    LightUniform m_lightUniform;
    int m_numLights;
    std::map<int, const Light *> m_lights;

    struct MaterialUniform {
        std::string name;
        std::map<std::string, GLint> locations;
    };

    MaterialUniform m_materialUniform;
    const Material *m_material;

    struct GenericUniform {
        std::string name;
        GLint locations;
    };
    std::map<UniformType, GenericUniform> m_genericUniforms;
};

}
#endif // SHADER_HPP
