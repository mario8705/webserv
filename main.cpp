#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "CommandLineOpt.h"
#include "SelectEventLoop.h"
#include "ListenerEvent.h"
#include "ConnectionHandler.h"
#include "DataBuffer.h"

class BufferShard
{
public:
   /* BufferShard(int fd)
        : m_fd(fd)
    {
        m_buffer = NULL;
        m_capacity = 0;
        m_readPos = 0;
        m_writePos = 0;
    }*/

    BufferShard(size_t capacity)
        : m_capacity(capacity)
    {
        m_buffer = new char[capacity];
        m_writePos = 0;
        m_readPos = 0;
    }

    ~BufferShard()
    {
        if (m_fd >= 0)
            close(m_fd);
        if (m_buffer)
            delete[] m_buffer;
    }

    int Write(const void *data, size_t n)
    {
        size_t space;

        space = GetAvailableSpace();
        printf("Space: %lu\n", space);
        if (n > space)
            n = space;
        if (n > 0) {
            std::memcpy(m_buffer + m_writePos, data, n);
            m_writePos += n;
            if (m_writePos == m_capacity)
                m_writePos = 0;
        }
        return n;
    }

    bool IsEmpty() const
    {
        return m_readPos == m_writePos;
    }

    bool IsFull() const
    {
        return ((m_writePos + 1) % m_capacity) == m_readPos;
    }

    int GetAvailableSpace() const
    {
        if (m_readPos <= m_writePos)
            return m_capacity - m_writePos;
        return m_readPos - m_writePos;
    }

    int m_fd;
    char *m_buffer;
    size_t m_capacity;
    size_t m_readPos;
    size_t m_writePos;
};

class OutputBuffer
{
public:

    int GetLength() const
    {
        return 0;
    }

    int Write(const void *data, size_t n)
    {
        return 0;
    }

private:
    std::vector<BufferShard *> m_shards;
};

class ServerHost;

class SocketEvent : public IOEventBase
{
public:
    SocketEvent(ServerHost *host, int fd);
    virtual ~SocketEvent();

    void HandleReadEvent();
    void HandleWriteEvent();

    bool IsWritable() const;

protected:
    ServerHost *m_host;
    DataBuffer *m_receiveBuffer;
    std::string m_outputBuffer;
};

int fd_set_non_blocking(int fd);

class ServerHost : public IConnectionHandler
{
public:
    ServerHost(IEventLoop *eventLoop)
        : m_eventLoop(eventLoop)
    {}

    void HandleConnection(int fd, struct sockaddr *addr, socklen_t addrlen)
    {
        SocketEvent *event;

        if (fd_set_non_blocking(fd) < 0)
        {
            close(fd);
            return ;
        }

        printf("New connection :D\n");
        event = new SocketEvent(this, fd);

        m_eventLoop->RegisterEvent(event);

        m_events.push_back(event);
    }

    void Disconnect(SocketEvent *event)
    {
        auto it = std::find(m_events.begin(), m_events.end(), event);

        if (it != m_events.end())
        {
            m_eventLoop->UnregisterEvent(event);

            m_events.erase(it);

            delete event;

            printf("Disconnected %p\n", event);
        }
    }

private:
    IEventLoop *m_eventLoop;
    std::vector<SocketEvent *> m_events;
};

SocketEvent::SocketEvent(ServerHost *host, int fd)
    : IOEventBase(fd), m_host(host)
{
    m_receiveBuffer = new DataBuffer(16384);
}

SocketEvent::~SocketEvent()
{
    delete m_receiveBuffer;
    close(m_fd);
}

void SocketEvent::HandleReadEvent()
{
    int n;
    std::string line;

    n = m_receiveBuffer->Receive(m_fd);
    if (n <= 0)
    {
        m_host->Disconnect(this);
    }
    else
    {
        while (m_receiveBuffer->GetLine(line))
        {
            printf("Line: %s (%d)\n", line.c_str(), line.size());
            if (line.empty())
            {
                m_outputBuffer = "HTTP/1.1 200 Ok\r\nContent-Type: text/html\r\n\r\nHello, World !";
            }
        }
    }
}

void SocketEvent::HandleWriteEvent()
{
    ssize_t n;

    n = send(m_fd, m_outputBuffer.c_str(), m_outputBuffer.size(), 0);
    if (n < 0)
    {
        m_host->Disconnect(this);
    }
    else
    {
        m_outputBuffer.erase(0, n);
    }
}

bool SocketEvent::IsWritable() const
{
    return m_outputBuffer.size() > 0;
}

int main(int argc, char *argv[])
{
/*    BufferShard shard(15);
    printf("Wrote: %d\n\n", shard.Write("Hello", 5));
    printf("Wrote: %d\n\n", shard.Write("Hello", 5));
    printf("Wrote: %d\n\n", shard.Write("Hello", 5));
    printf("Wrote: %d\n\n", shard.Write("Hello", 5));

    return 0;*/
    CommandLineOpt opt;
    SelectEventLoop eventLoop;

    opt.Parse(argc, argv);

    struct sockaddr_in sin;

    sin.sin_family = AF_INET;
    sin.sin_port = htons(8080);
    sin.sin_addr.s_addr = INADDR_ANY;

    ServerHost *serverHost = new ServerHost(&eventLoop);

    ListenerEvent *serv = ListenerEvent::CreateAndBind(serverHost, (struct sockaddr *) &sin, sizeof(sin), 10);
    eventLoop.RegisterEvent(serv);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(8082);
    sin.sin_addr.s_addr = INADDR_ANY;

    serv = ListenerEvent::CreateAndBind(serverHost, (struct sockaddr *) &sin, sizeof(sin), 10);
    eventLoop.RegisterEvent(serv);

    eventLoop.Run();
    return 0;
}