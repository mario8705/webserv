//
// Created by alavaud on 3/13/23.
//

#ifndef WEBSERV_EVENTLOOP_H
#define WEBSERV_EVENTLOOP_H

class IIOEvent;

class IEventLoop
{
public:
    virtual ~IEventLoop() {}

    virtual void RegisterEvent(IIOEvent *evt) = 0;
    virtual void UnregisterEvent(IIOEvent *evt) = 0;

    virtual void LoopOnce() = 0;
};


#endif //WEBSERV_EVENTLOOP_H
