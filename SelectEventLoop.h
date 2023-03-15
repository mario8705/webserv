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
    virtual ~SelectEventLoop();

    virtual void RegisterEvent(IIOEvent *evt);
    virtual void UnregisterEvent(IIOEvent *evt);
    
    virtual void Run();

private:
    tEventMap m_eventMap;
};


#endif //WEBSERV_SELECTEVENTLOOP_H
