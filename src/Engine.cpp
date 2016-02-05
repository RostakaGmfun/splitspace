#include <splitspace/Engine.hpp>
#include <splitspace/LogManager.hpp>
#include <splitspace/EventManger.hpp>
#include <splitspace/WindowManager.hpp>
#include <splitspace/SceneManager.hpp>
#include <splitspace/RenderManager.hpp>
#include <splitspace/ResourceManager.hpp>
#include <splitspace/PhysicsManager.hpp>
#include <splitspace/Config.hpp>

#include <iostream>
#include <chrono>

namespace splitspace {

Engine::Engine():   logManager(nullptr),
                    eventManager(nullptr),
                    windowManager(nullptr),
                    resManager(nullptr),
                    sceneManager(nullptr),
                    renderManager(nullptr),
                    physManager(nullptr),
                    config(nullptr),
                    m_quit(false)
{}

Engine::~Engine() {
    destroyManagers();
}

bool Engine::init() {
    config = new Config();
    if(!config->parse("data/main.json")) {
        std::cerr << "Error parsing data/main.json" << std::endl;
        return false;
    }
    if(!initLog())
        return false;
    if(!initEvents())
        return false;
    if(!initWindow())
        return false;
    if(!initResources())
        return false;
    if(!initScenes())
        return false;
    if(!initRendering())
        return false;
    if(!initPhysics())
        return false;

    return true;
}

void Engine::mainLoop() {
    using namespace std::chrono;
    Event *e = new UpdateEvent(0);
    milliseconds cur, last = duration_cast<milliseconds>(
                        system_clock::now().time_since_epoch());
    while(!m_quit) {
        cur = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
        windowManager->collectEvents();
        eventManager->emitEvent(e);
        sceneManager->update((cur.count()-last.count())/1000.f);
        renderManager->render();
        last = cur;
    }
    
    eventManager->logStats();
    renderManager->logStats();
}

bool Engine::initLog() {
    logManager = new LogManager(config->log.logFile);
    if(!logManager) {
        std::cout << "(Engine) Memory error" << std::endl;
        return false;
    }
    logManager->setLevel(config->log.level);
    return true;
}

bool Engine::initEvents() {
    eventManager = new EventManager(logManager);
    if(!eventManager) {
        logManager->logErr("(Engine) Memory error");
        return false;
    }
    return true;
}

bool Engine::initWindow() {
    windowManager = new WindowManager(this);
    if(!windowManager) {
        logManager->logErr("(Engine) Memory error");
        return false;
    }
    if(!windowManager->init(config->window.caption.c_str(), config->window.width,
                            config->window.height, config->window.fullscreen)) {
        logManager->logErr("(Engine) Error initalising WindowManager");
        return false;
    }
    return true;
}

bool Engine::initResources() {
    resManager = new ResourceManager(this);
    if(!resManager) {
        logManager->logErr("(Engine) Out of memory");
        return false;
    }

    return resManager->loadMaterialLib(config->matLibs);
}

bool Engine::initScenes() {
    sceneManager = new SceneManager(this); 
    if(!sceneManager) {
        logManager->logErr("(Engine) Memory error");
        return false;
    }
    return sceneManager->init(config->scenes);
}

bool Engine::initRendering() {
    renderManager = new RenderManager(this);
    if(!renderManager) {
        logManager->logErr("(Engine) Memory error");
        return false;
    }

    return renderManager->init(config->window.vsync);
}

bool Engine::initPhysics() {

    return true;
}

void Engine::shutdown() {
    m_quit = true;
}

void Engine::destroyManagers() {
    if(windowManager)
        delete windowManager;
    if(resManager)
        delete resManager;
    if(sceneManager)
        delete sceneManager;
    if(renderManager)
        delete renderManager;
    if(physManager)
        delete physManager;
    if(logManager)
        delete logManager;
    if(eventManager)
        delete eventManager;
}

} // namespace splitspace

