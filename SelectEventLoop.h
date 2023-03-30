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
    typedef std::map<int, IIOEvent *> tEventMap;

    SelectEventLoop();
    ~SelectEventLoop();

    void RegisterEvent(IIOEvent *evt);
    void UnregisterEvent(IIOEvent *evt);
    
    void LoopOnce();

private:
    tEventMap m_eventMap;
};


#endif //WEBSERV_SELECTEVENTLOOP_H
