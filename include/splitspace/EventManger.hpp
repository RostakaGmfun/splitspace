#ifndef EVENT_MANAGER_HPP
#define EVENT_MANAGER_HPP

#include <vector>
#include <inttypes.h>
#include <string>
#include <SDL2/SDL_scancode.h>

namespace splitspace {

enum EventMask {
    EVM_GAME = 1<<1,
    EVM_WINDOW  = 1<<2,
    EVM_INPUT = 1<<3,
};

enum EventType {
    //EVM_GAME
    EV_UPDATE,

    //EVM_WINDOW
    EV_RENDER,
    EV_WINDOW_RESIZE,
    EV_QUIT,

    //EVM_INPUT
    EV_KEY,
    EV_MOUSE
};

struct Event {
    Event(EventType t, EventMask m): type(t), mask(m)
    {}

    EventType type;
    int32_t mask;
};

enum KeyState {
    KEY_PRESS,
    KEY_RELEASE
};

struct KeyboardEvent: public Event {
    KeyboardEvent(SDL_Scancode sc, KeyState st):
            Event(EV_KEY, EVM_INPUT), 
            scancode(sc), state(st)
    {}
    SDL_Scancode scancode;
    KeyState state;
};

enum MouseButton {
    MOUSE_LEFT,
    MOUSE_RIGHT,
    MOUSE_MIDDLE
};

struct MouseEvent: public Event {
    MouseEvent(MouseButton b, KeyState s):
         Event(EV_MOUSE, EVM_INPUT),
         button(b), state(s)
    {}
    MouseButton button;
    KeyState state;
};

struct UpdateEvent: public Event {
    UpdateEvent(float dt):
          Event(EV_UPDATE, EVM_GAME),
          delta(dt)
    {}
    float delta;
};

class EventListener {
public:
    EventListener(int32_t mask, const std::string &name): m_mask(mask), m_name(name)
    {}
    virtual ~EventListener() {}

    int32_t getMask() const { return m_mask; }

    virtual void onEvent(Event *event) = 0;

    const std::string &getName() const { return m_name; }

protected:
    int32_t m_mask;
    std::string m_name;
};

class LogManager;

class EventManager {
public:
    EventManager(LogManager *lm);
    ~EventManager() {}

    bool addListener(EventListener *l);
    bool emitEvent(Event *ev);

    void logStats();

private:
    std::string getMaskName(int32_t mask);

private:
    std::vector<EventListener *> m_listeners;
    LogManager *m_logManager;
    int m_numEventsDispatched;
};


} // namepsace splitspace
#endif // EVENT_MANAGER_HPP
