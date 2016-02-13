#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <splitspace/EventManager.hpp>

#include <SDL2/SDL.h>
#include <vector>

namespace splitspace {

class Engine;
class LogManager;

class WindowManager {
public:
    WindowManager(Engine *e);
    ~WindowManager();

    bool init(const char *caption, int w, int h, bool fs);

    void collectEvents();

    void destroy();

    SDL_Window *getSDLWindow() const { return m_window; }

private:
    EventManager *m_evtMan;
    LogManager *m_logMan;
    SDL_Window *m_window;
    std::vector<SDL_Scancode> m_keysDown;
    bool m_leftButtonDown;
    bool m_rightButtonDown;
    bool m_middleButtonDown;
};

} // namespace splitspace

#endif // WINDOW_MANAGER_H
