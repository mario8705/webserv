//
// Created by alavaud on 3/14/23.
//

#include "SelectEventLoop.h"
#include <cstdlib>
#include <sys/types.h>
#include <sys/select.h>
#include "IOEvent.h"
#include <vector>

SelectEventLoop::SelectEventLoop()
{
}

SelectEventLoop::~SelectEventLoop()
{
}

void SelectEventLoop::RegisterEvent(IIOEvent *evt)
{
    int fd;
    tEventMap::iterator it;

    fd = evt->GetFileDescriptor();
    it = m_eventMap.find(fd);
    if (it == m_eventMap.end())
    {
        m_eventMap.insert(std::make_pair(fd, evt));
    }
}

void SelectEventLoop::UnregisterEvent(IIOEvent *evt)
{
    tEventMap::iterator it;

    it = m_eventMap.find(evt->GetFileDescriptor());
    if (it != m_eventMap.end())
    {
        printf("UnregisterEvent(%p)\n", evt);
        m_eventMap.erase(it);
    }
}

void SelectEventLoop::Run()
{
    fd_set rdset, wrset;
    IIOEvent *evt;
    int fd;
    int nfds;
    int n;
    tEventMap::iterator it;
    std::vector<IIOEvent *> readEvents;
    std::vector<IIOEvent *> writeEvents;
    size_t i;

    while (1)
    {
        FD_ZERO(&rdset);
        FD_ZERO(&wrset);
        readEvents.clear();
        writeEvents.clear();

        nfds = 0;
        for (it = m_eventMap.begin(); it != m_eventMap.end(); ++it)
        {
            fd = it->first;
            evt = it->second;

            if (fd > nfds)
                nfds = fd;
            FD_SET(fd, &rdset);
            if (evt->IsWritable())
                FD_SET(fd, &wrset);
        }
        n = select(nfds + 1, &rdset, &wrset, NULL, NULL);
        if (n < 0)
        {
            return ;
        }
        for (it = m_eventMap.begin(); it != m_eventMap.end(); ++it)
        {
            fd = it->first;
            evt = it->second;
            if (FD_ISSET(fd, &rdset))
                readEvents.push_back(evt);
            if (FD_ISSET(fd, &wrset))
                writeEvents.push_back(evt);
        }
        for (i = 0; i < readEvents.size(); ++i)
            readEvents[i]->HandleReadEvent();
        for (i = 0; i < writeEvents.size(); ++i)
            writeEvents[i]->HandleWriteEvent();
    }
}
