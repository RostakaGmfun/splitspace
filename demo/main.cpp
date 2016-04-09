#include <splitspace/Engine.hpp>
#include <splitspace/Config.hpp>
#include <splitspace/EventManager.hpp>
#include <splitspace/ResourceManager.hpp>
#include <splitspace/RenderManager.hpp>
#include <splitspace/Scene.hpp>
#include <splitspace/Camera.hpp>
#include <splitspace/Entity.hpp>

#include <iostream>

using namespace splitspace;

class Demo: public EventListener {
public:
    Demo(): EventListener(EVM_WINDOW|EVM_INPUT, "MyListener"),
                  m_engine(nullptr), m_scene(nullptr)
    {}

    int run() {
        m_engine = new Engine();
        if(!m_engine->init()) {
            return 1;
        }

        m_scene = static_cast<Scene *>(m_engine->resManager->loadResource("demoScene"));
        if(!m_scene) {
            return 1;
        }

        m_camera = new FPSCamera(m_engine->config->window.width,
                                 m_engine->config->window.height,
                                 45.f, 1.0f, 1000.f);

        m_engine->renderManager->setCamera(m_camera);
        m_engine->eventManager->addListener(this);
        m_engine->mainLoop();

        delete m_camera;
        return 0;
    }

    virtual void onEvent(Event *e) {
        switch(e->type) {
            case EV_QUIT:
                m_engine->shutdown();
            break;
            case EV_KEY:
                handleKey(static_cast<KeyboardEvent *>(e));
            break;
            case EV_MOUSE:
                handleMouse(static_cast<MouseEvent *>(e));
            break;
            case EV_UPDATE:
                handleUpdate(static_cast<UpdateEvent *>(e));
            break;
            default:
            break;
        }
    }

private:
    void handleKey(const KeyboardEvent *kbev) {

    }

    void handleMouse(const MouseEvent *mev) {

    }

    void handleUpdate(const UpdateEvent *uev) {
        m_camera->update(uev->delta);
        Entity *root = m_scene->getRootNode();
        if(root) {
            root->update(uev->delta);
        }
    }

private:
    Engine *m_engine;
    Scene *m_scene;
    Camera *m_camera;
};

int main() {
    Demo d;
    return d.run();
}
