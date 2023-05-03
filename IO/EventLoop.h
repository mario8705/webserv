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

    /**
     * Block until at least one event occured or a signal interrupts the handler.
     * @return true if an error occured (including a caught signal)
     */
    virtual bool LoopOnce() = 0;
};


#endif //WEBSERV_EVENTLOOP_H
