//
// Created by alavaud on 3/14/23.
//

#ifndef WEBSERV_SELECTEVENTLOOP_H
#define WEBSERV_SELECTEVENTLOOP_H
#include "EventLoop.h"
#include <vector>
#include <map>

class SelectEventLoop : public IEventLoop
{
public:
    typedef std::map<int, IOEvent *> tEventMap;
    typedef std::vector<IOEvent *> tRaisedEventList;

    SelectEventLoop();
    ~SelectEventLoop();

    void RegisterEvent(IOEvent *evt);
    void UnregisterEvent(IOEvent *evt);
    
    bool LoopOnce();

    void RaiseReadEvent(IOEvent *evt);

private:
    void ProcessPendingEvents();

    tEventMap m_eventMap;
    tRaisedEventList m_pendingReads;
    tRaisedEventList m_pendingWrites;
};


#endif //WEBSERV_SELECTEVENTLOOP_H
