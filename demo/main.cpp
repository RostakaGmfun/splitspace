#include <splitspace/Engine.hpp>
#include <splitspace/EventManger.hpp>
#include <splitspace/ResourceManager.hpp>

#include <iostream>

using namespace splitspace;
Engine engine;

class MyListener: public EventListener {
public:
    MyListener(): EventListener(EVM_WINDOW|EVM_INPUT, "MyListener")
    {}
    virtual void onEvent(Event *e) {
        if(e->type == EV_QUIT)
            engine.shutdown();
        if(e->type == EV_KEY) {
            std::cout << "Key " << ((static_cast<KeyboardEvent *>(e))->state == KEY_PRESS?"pressed":"released") << std::endl;
        }
        if(e->type == EV_MOUSE) {
            std::cout << "Mouse button " << ((static_cast<MouseEvent *>(e))->state == KEY_PRESS?"pressed":"released") << std::endl;
        }
    }
};

int main() {
    if(!engine.init())
        return 1;
    if(!engine.resManager->loadResource("demoScene"))
        return 1;

    engine.eventManager->addListener(new MyListener()); 
    engine.mainLoop();
    return 0;
}
