#include <splitspace/WindowManager.hpp>
#include <splitspace/Engine.hpp>
#include <splitspace/LogManager.hpp>

#include <algorithm>
#include <iostream>

namespace splitspace {
WindowManager::WindowManager(Engine *e): m_evtMan(e->eventManager),
                                         m_logMan(e->logManager),
                                         m_window(nullptr),
                                         m_leftButtonDown(false),
                                         m_rightButtonDown(false),
                                         m_middleButtonDown(false)
{}

WindowManager::~WindowManager() {
    destroy();
}

bool WindowManager::init(const char *caption, int w, int h, bool fs) {
    if(w<=0 || h<=0) {
        m_logMan->logErr("(WindowManager) Invalid window size given");
        return false;
    }

    Uint32 flags = SDL_WINDOW_OPENGL | (fs & SDL_WINDOW_FULLSCREEN);
    m_window = SDL_CreateWindow(caption, SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, w, h, flags);
    if(!m_window) {
        m_logMan->logErr("(WindowManager) Couldn't create window: "
                         +std::string(SDL_GetError()));
        return false;
    }

    m_logMan->logInfo("(WindowManager) Created "+std::string(fs?"fullscreen ":"")
                      +"window with size "+std::to_string(w)+"x"+std::to_string(h));

    return true;
}

void WindowManager::destroy() {
    if(m_window)
        SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void WindowManager::collectEvents() {
    SDL_Event ev;
    Event *evPtr;
    while(SDL_PollEvent(&ev)) {
        switch(ev.type) {
            case SDL_QUIT: {
                evPtr = new Event(EV_QUIT, EVM_WINDOW);
                m_evtMan->emitEvent(evPtr);
                delete evPtr;
                break;
            }
            case SDL_KEYDOWN: {
                if(std::find(m_keysDown.begin(), m_keysDown.end(),
                        ev.key.keysym.scancode)==m_keysDown.end()) {
                    m_keysDown.push_back(ev.key.keysym.scancode);
                    evPtr = new KeyboardEvent(ev.key.keysym.scancode, KEY_PRESS);
                    m_evtMan->emitEvent(evPtr);
                    delete evPtr;
                }
                break;
            }
            case SDL_KEYUP: {
                auto k = std::find(m_keysDown.begin(), m_keysDown.end(),
                                                  ev.key.keysym.scancode);
                if(k!=m_keysDown.end()) {
                    m_keysDown.erase(k);
                    evPtr = new KeyboardEvent(ev.key.keysym.scancode, KEY_RELEASE);
                    m_evtMan->emitEvent(evPtr);
                    delete evPtr;
                }
                break;
            }
            case SDL_MOUSEBUTTONDOWN: {
                switch(ev.button.button) {
                    case SDL_BUTTON_LEFT:
                        if(!m_leftButtonDown) {
                            m_leftButtonDown = true;
                            evPtr = new MouseEvent(MOUSE_LEFT, KEY_PRESS);
                            m_evtMan->emitEvent(evPtr);
                            delete evPtr;
                        }
                        break;
                    case SDL_BUTTON_RIGHT:
                        if(!m_rightButtonDown) {
                            m_rightButtonDown = true;
                            evPtr = new MouseEvent(MOUSE_RIGHT, KEY_PRESS);
                            m_evtMan->emitEvent(evPtr);
                            delete evPtr;
                        }
                        break;
                    case SDL_BUTTON_MIDDLE:
                        if(!m_middleButtonDown) {
                            m_middleButtonDown = true;
                            evPtr = new MouseEvent(MOUSE_MIDDLE, KEY_PRESS);
                            m_evtMan->emitEvent(evPtr);
                            delete evPtr;
                        }
                        break;
                }
                break;
            }
            case SDL_MOUSEBUTTONUP: {
                switch(ev.button.button) {
                    case SDL_BUTTON_LEFT:
                        m_leftButtonDown = false;
                        evPtr = new MouseEvent(MOUSE_LEFT, KEY_RELEASE);
                        m_evtMan->emitEvent(evPtr);
                        delete evPtr;
                        break;
                    case SDL_BUTTON_RIGHT:
                        m_rightButtonDown = false;
                        evPtr = new MouseEvent(MOUSE_RIGHT, KEY_RELEASE);
                        m_evtMan->emitEvent(evPtr);
                        delete evPtr;
                        break;
                    case SDL_BUTTON_MIDDLE:
                        m_middleButtonDown = false;
                        evPtr = new MouseEvent(MOUSE_MIDDLE, KEY_RELEASE);
                        m_evtMan->emitEvent(evPtr);
                        delete evPtr;
                        break;
                }
                break;
            }
        }
    }
}

} // namespace splitspace
