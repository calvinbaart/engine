#ifndef _EVENT_HANDLER_H_
#define _EVENT_HANDLER_H_

#include "Config.h"

class EventHandler
{
private:
    vector<EventPtr> m_events;
public:
    EventHandler();
    virtual ~EventHandler();

    EventPtr get_event();
    void fire_event(EventPtr evt);
};

#endif