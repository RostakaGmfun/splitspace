#include <splitspace/EventManger.hpp>
#include <splitspace/LogManager.hpp>

#include <algorithm>

namespace splitspace {

EventManager::EventManager(LogManager *lm): m_logManager(lm), m_numEventsDispatched(0)
{
    m_listeners.clear();
}

bool EventManager::addListener(EventListener *l) {
    if(!l)
        return false;
    if(std::find(m_listeners.begin(), m_listeners.end(), l)!=m_listeners.end())
        return false;
    m_listeners.push_back(l);
    m_logManager->logInfo("(EventManager) Registered event listener "+
                          l->getName()+" with mask "+getMaskName(l->getMask()));

    return true;
}

std::string EventManager::getMaskName(int32_t mask) {
    std::string str = "";
    if(mask & EVM_GAME)
        str+="EVM_GAME ";
    if(mask & EVM_WINDOW)
        str+="EVM_WINDOW ";
    if(mask & EVM_INPUT)
        str+="EVM_INPUT ";
    return str;
}

bool EventManager::emitEvent(Event *ev) {
    if(ev->mask == 0)
        return false;
    m_numEventsDispatched++;
    for(auto i = m_listeners.begin();i!=m_listeners.end();i++) {
        if(!(*i)) {
            m_logManager->logErr("(EventManager) Encountered nullptr EventListener!");
            continue;
        }
        if((*i)->getMask() & ev->mask)
            (*i)->onEvent(ev);
    }

    return true;
}

void EventManager::logStats() {
    m_logManager->logInfo("(EventManager) STATS:");
    m_logManager->logInfo("\t Number of event listeners: "
                          +std::to_string(m_listeners.size()));
    m_logManager->logInfo("\t Number of events dispatched: "
                          +std::to_string(m_numEventsDispatched));
}

} // namespace splitspace

