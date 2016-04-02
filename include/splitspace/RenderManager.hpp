#ifndef RENDER_MANAGER_HPP
#define RENDER_MANAGER_HPP

#include <SDL2/SDL.h>
#include <vector>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace splitspace {

class Engine;
class WindowManager;
class LogManager;
class ResourceManager;
class SceneManager;
class Scene;

class Texture;
class Shader;
class Mesh;
class Object;
class Material;

enum VertexFormat {
    VERTEX_UNKNOWN,
    VERTEX_3DT,
    VERTEX_3DN,
    VERTEX_3DTN
};

enum ImageFormat {
    IMAGE_R,
    IMAGE_RGB,
    IMAGE_RGBA,
    IMAGE_UNKNOWN,
};

enum TextureFiltering {
    TEX_FILTER_NEAREST,
    TEX_FILTER_LINEAR
};

enum TextureUsage {
    TEX_DIFFUSE,
    TEX_NORMAL,
    TEX_RENDERTARGET
};

struct Vertex3DT {
    glm::vec3 pos;
    glm::vec2 texcoord;
};

struct Vertex3DN {
    glm::vec3 pos;
    glm::vec3 normal;
};

struct Vertex3DTN {
    glm::vec3 pos;
    glm::vec2 texcoord;
    glm::vec3 normal;
};

struct MeshData;
struct TextureData;
struct ShaderData;
struct MaterialData;
struct ObjectData;
struct LightData;

class RenderManager {
public:
    RenderManager(Engine *e);
    ~RenderManager();

    bool init(bool vsync);
    void render();
    void destroy();

    void setScene(Scene *scn) { m_scene = scn; }

    bool createTexture(const void *data, ImageFormat format, int w, int h, GLuint &glName);
    bool createSampler(bool useMipmaps, TextureFiltering filtering, GLuint &smaplerName);
    bool createMesh(const void *vData, VertexFormat format, int numVerts, GLuint &vboName, GLuint &vaoName);
    bool createShader(const char *vsSrc, const char *fsSrc,int vsVer, int fsVer,
                      VertexFormat inputFormat, const int numOutputs, GLuint &glName);

    void destroyMesh(GLuint &vao, GLuint &vbo);
    void destroyTexture(GLuint &texId);
    void destroySampler(GLuint &sampler);
    void destroyShader(GLuint &progId);

    void logStats();
    int getFrameDrawCalls() const { return m_frameDrawCalls; }
    int getTotalDrawCalls() const { return m_totalDrawCalls; }

private:
    void setupGL();

    bool createVAOAndVBO(GLuint &vao, GLuint &vbo);
    void destroyVAOAndVBO(GLuint &vao, GLuint &vbo);

    bool compileShader(GLuint shader, const char *src, int ver);
    bool linkProgram(GLuint program, GLuint vs, GLuint fs);

    std::size_t getTextureSize(const GLuint texId);

    void beginFrame();
    void renderScene();
    void endFrame();

    void setupMaterial(const Material *m);
    void setupMesh(const Mesh *m);
    void drawCall();

private:
    WindowManager *m_winManager;
    LogManager *m_logManager;
    ResourceManager *m_resManager;
    SDL_GLContext m_context;
    SDL_Window *m_window;

    int m_frameDrawCalls;
    int m_totalDrawCalls;
    int m_totalShaders;
    int m_totalMeshes;
    int m_totalTextures;
    int m_totalFrames;
    float m_averageFrameTime;
    int m_memoryUsed;

    Scene *m_scene;

};

} // namespace splitspace

#endif // RENDER_MANAGER_HPP
