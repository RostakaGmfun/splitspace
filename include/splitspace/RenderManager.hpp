#ifndef RENDER_MANAGER_HPP
#define RENDER_MANAGER_HPP

#include <SDL2/SDL.h>
#include <vector>
#include <GL/glew.h>
#include <GL/gl.h>

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

enum VertexFormat {
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


class RenderManager {
public:
    RenderManager(Engine *e);
    ~RenderManager();

    bool init(bool vsync);
    bool loadScene(Scene *s);
    void unloadScene();
    void render();
    void destroy();

    bool createTexture(void *data, bool useMipmaps, ImageFormat format, int w, int h, GLuint &glName);
    bool createSampler(bool useMipmaps, TextureFiltering filtering, GLuint &smaplerName);
    bool createShader(const char *vs, const char *fs, std::vector<ImageFormat> outFormat, GLuint &glName);
    bool createMesh(void *vData, VertexFormat format, int numVerts, Mesh *mesh, GLuint &vboName);
    bool createMesh(void *vData, void *iData, VertexFormat format, int numVerts, int numInds, GLuint &vboName, GLuint &iboName);

    void logStats();
    int getFrameDrawCalls() const { return m_frameDrawCalls; }
    int getTotalDrawCalls() const { return m_totalDrawCalls; }

private:
    void setupGL();

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

};

} // namespace splitspace

#endif // RENDER_MANAGER_HPP
