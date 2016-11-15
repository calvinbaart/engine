#include "EventHandler.h"

EventHandler::EventHandler()
{
}

EventHandler::~EventHandler()
{
}

EventPtr EventHandler::get_event()
{
    if (m_events.empty()) return nullptr;

    EventPtr evt = m_events[0];
    m_events.erase(m_events.begin());

    return evt;
}

void EventHandler::fire_event(EventPtr evt)
{
    m_events.push_back(evt);
}
