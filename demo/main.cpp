#include <splitspace/Engine.hpp>
#include <splitspace/EventManager.hpp>
#include <splitspace/ResourceManager.hpp>
#include <splitspace/RenderManager.hpp>
#include <splitspace/Scene.hpp>

#include <iostream>

using namespace splitspace;
Engine engine;

class MyListener: public EventListener {
public:
    MyListener(): EventListener(EVM_WINDOW|EVM_INPUT, "MyListener")
    {}
    virtual void onEvent(Event *e) {
        if(e->type == EV_QUIT) {
            engine.shutdown();
        }
        if(e->type == EV_KEY) {
            std::cout << "Key " << ((static_cast<KeyboardEvent *>(e))->state == KEY_PRESS?"pressed":"released") << std::endl;
        }
        if(e->type == EV_MOUSE) {
            std::cout << "Mouse button " << ((static_cast<MouseEvent *>(e))->state == KEY_PRESS?"pressed":"released") << std::endl;
        }
    }
};

int main() {
    if(!engine.init()) {
        return 1;
    }

    Scene *scene = static_cast<Scene *>(engine.resManager->loadResource("demoScene"));
    if(!scene) {
        return 1;
    }
    
    engine.renderManager->setScene(scene);

    std::cout << engine.resManager->printManifests();
    std::cout << engine.resManager->printResources();

    engine.eventManager->addListener(new MyListener()); 
    engine.mainLoop();
    return 0;
}
