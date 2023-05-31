//
// Created by alavaud on 3/14/23.
//

#include "SelectEventLoop.h"
#include <cstdlib>
#include <sys/types.h>
#include <sys/select.h>
#include "IOEvent.h"
#include <vector>
#include <iostream>
#include <errno.h>
#include <cstdio>

SelectEventLoop::SelectEventLoop()
{
}

SelectEventLoop::~SelectEventLoop()
{
}

void SelectEventLoop::RegisterEvent(IOEvent *evt)
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

void SelectEventLoop::UnregisterEvent(IOEvent *evt)
{
    tEventMap::iterator it;
    tRaisedEventList::iterator it2;

    /* Remove from pending events list */
    for (it2 = m_pendingReads.begin(); it2 != m_pendingReads.end(); ++it2)
    {
        if (*it2 == evt) {
            m_pendingReads.erase(it2);
            break ;
        }
    }
    for (it2 = m_pendingWrites.begin(); it2 != m_pendingWrites.end(); ++it2)
    {
        if (*it2 == evt) {
            m_pendingWrites.erase(it2);
            break ;
        }
    }

    it = m_eventMap.find(evt->GetFileDescriptor());
    if (it != m_eventMap.end())
    {
        m_eventMap.erase(it);
    }
}

bool SelectEventLoop::LoopOnce()
{
    fd_set rdset, wrset;
    IOEvent *evt;
    int fd;
    int nfds;
    int n;
    tEventMap::iterator it;

    FD_ZERO(&rdset);
    FD_ZERO(&wrset);
    nfds = 0;
    for (it = m_eventMap.begin(); it != m_eventMap.end(); ++it)
    {
        fd = it->first;
        evt = it->second;

        if (fd > nfds)
            nfds = fd;
        if (evt->IsReadable())
            FD_SET(fd, &rdset);
        if (evt->IsWritable())
            FD_SET(fd, &wrset);
    }
    do {
        n = select(nfds + 1, &rdset, &wrset, NULL, NULL);
    } while (n < 0 && errno == EINTR);
    if (n < 0)
    {
        perror("select");
        return true;
    }
    for (it = m_eventMap.begin(); it != m_eventMap.end(); ++it)
    {
        fd = it->first;
        evt = it->second;
        if (FD_ISSET(fd, &rdset))
            m_pendingReads.push_back(evt);
        if (FD_ISSET(fd, &wrset))
            m_pendingWrites.push_back(evt);
    }
    ProcessPendingEvents();
    return false;
}

void SelectEventLoop::RaiseReadEvent(IOEvent *evt)
{
    size_t i;

    /*
     * Avoid calling the callback twice as it can lead to blocking reads.
     */
    for (i = 0; i < m_pendingReads.size(); ++i)
    {
        if (m_pendingReads[i] == evt)
            return ;
    }
    m_pendingReads.push_back(evt);
}

void SelectEventLoop::ProcessPendingEvents()
{
    IOEvent *evt;
    size_t i;

    for (i = 0; i < m_pendingReads.size(); ++i)
    {
        evt = m_pendingReads[i];
        evt->NotifyRead();
    }
    for (i = 0; i < m_pendingWrites.size(); ++i)
    {
        evt = m_pendingWrites[i];
        evt->NotifyWrite();
    }
    m_pendingReads.clear();
    m_pendingWrites.clear();
}
