#ifndef RENDER_TECHNIQUE_HPP
#define RENDER_TECHNIQUE_HPP

#include <splitspace/Engine.hpp>
#include <splitspace/LogManager.hpp>

#include <vector>

namespace splitspace {

class Engine;
class LogManager;
class RenderManager;
class ResourceManager;

class Scene;
class Camera;
class Shader;
class Material;
class Mesh;

class RenderTechnique {
public:
    RenderTechnique(Engine *e): m_engine(e),
                                m_renderManager(e->renderManager),
                                m_logManager(e->logManager),
                                m_resManager(e->resManager)
    {}

    virtual ~RenderTechnique() {}

    virtual bool init() = 0;
    virtual void update(float dt) = 0;
    virtual void render() = 0;

    virtual void destroy() = 0;

    void setScene(Scene *scene) { m_scene = scene; }
    Scene *getScene() const { return m_scene; }

    void setViewCamera(Camera *camera) { m_viewCamera = camera; }
    Camera *getViewCamera() const { return m_viewCamera; }

protected:
    bool setupMaterial(Shader *shader, const Material *material);
    bool setupMesh(Shader *shader, const Mesh *mesh);

    void drawCall(std::size_t numVerts);

protected:
    Engine *m_engine;
    RenderManager *m_renderManager;
    LogManager *m_logManager;
    ResourceManager *m_resManager;
    Scene *m_scene;
    Camera *m_viewCamera;
};

} // namepsace splitspace

#endif // RENDER_TECHNIQUE_HPP
