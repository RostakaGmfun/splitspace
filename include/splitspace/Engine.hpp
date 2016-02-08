#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <string>

namespace splitspace {

class EventManager;
class EventListener;
class EventEmitter;
class WindowManager;
class LogManager;
class SceneManager;
class ResourceManager;
class RenderManager;
class PhysicsManager;
class Config;

class Engine {
public:
    Engine();
    ~Engine();

    bool init();

    void mainLoop();

    void shutdown();

private:
    bool initLog();
    bool initEvents();
    bool initWindow();
    bool initResources();
    bool initRendering();
    bool initPhysics();

    void destroyManagers();

public:

    LogManager *logManager;
    EventManager *eventManager;
    WindowManager *windowManager;
    ResourceManager *resManager;
    RenderManager *renderManager;
    PhysicsManager *physManager;

    Config *config;

private:

    bool m_quit;

};

} // namespace splitspace

#endif // ENGINE_HPP
