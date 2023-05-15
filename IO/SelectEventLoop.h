//
// Created by alavaud on 3/14/23.
//

#ifndef WEBSERV_SELECTEVENTLOOP_H
#define WEBSERV_SELECTEVENTLOOP_H
#include "EventLoop.h"
#include <map>

class SelectEventLoop : public IEventLoop
{
public:
    typedef std::map<int, IOEvent *> tEventMap;

    SelectEventLoop();
    ~SelectEventLoop();

    void RegisterEvent(IOEvent *evt);
    void UnregisterEvent(IOEvent *evt);
    
    bool LoopOnce();

private:
    tEventMap m_eventMap;
};


#endif //WEBSERV_SELECTEVENTLOOP_H
