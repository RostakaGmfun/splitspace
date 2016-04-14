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

class RenderTechnique {
public:
    RenderTechnique(Engine *e): m_renderManager(e->renderManager),
                                m_logManager(e->logManager),
                                m_resManager(e->resManager)
    {}

    virtual ~RenderTechnique() {}

    virtual bool init() = 0;
    virtual void update(float dt) = 0;
    virtual void render() {
        for(size_t i = 0;i<m_passes.size();i++) {
            if(m_passes[i]) {
                doPass(i);
            }
        }
    }

    virtual void destroy() = 0;

    bool addPass(int passId, Shader *shader) {
        if(static_cast<size_t>(passId)>=m_passes.capacity()) {
            return false;
        }

        m_passes[passId] = shader;
        return true;
    }

    void setScene(Scene *scene) { m_scene = scene; }
    Scene *getScene() const { return m_scene; }

    void setViewCamera(Camera *camera) { m_viewCamera = camera; }
    Camera *getViewCamera() const { return m_viewCamera; }

    const int MAX_PASSES = 8;

protected:
    virtual void doPass(int passId) = 0;

protected:
    RenderManager *m_renderManager;
    LogManager *m_logManager;
    ResourceManager *m_resManager;
    Scene *m_scene;
    Camera *m_viewCamera;

    std::vector<Shader*> m_passes;
};

} // namepsace splitspace

#endif // RENDER_TECHNIQUE_HPP
