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
#include "FileEvent.h"

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
    DataBuffer *m_outputBuffer;
    FileEvent *m_fileEvent;
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

    IEventLoop *GetEventLoop() const { return m_eventLoop; }

private:
    IEventLoop *m_eventLoop;
    std::vector<SocketEvent *> m_events;
};

SocketEvent::SocketEvent(ServerHost *host, int fd)
    : IOEventBase(fd), m_host(host)
{
    m_receiveBuffer = new DataBuffer();
    m_outputBuffer = new DataBuffer();
    m_fileEvent = NULL;
}

SocketEvent::~SocketEvent()
{
    if (m_fileEvent)
        delete m_fileEvent;
    delete m_receiveBuffer;
    delete m_outputBuffer;
    ::close(m_fd);
}

void SocketEvent::HandleReadEvent()
{
    int n;
    std::string line;

    n = m_receiveBuffer->Receive(m_fd);
    if (n <= 0)
    {
        printf("recv() <= 0\n");
        m_host->Disconnect(this);
    }
    else
    {
        while (m_receiveBuffer->Readln(line))
        {
            if (line.empty())
            {
                int fd;

                fd = open("index.html", O_RDONLY | O_NONBLOCK);
                perror("open");
                m_fileEvent = new FileEvent(m_host->GetEventLoop(), fd);

                char s[] = "HTTP/1.1 200 Ok\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n";
                m_outputBuffer->Write(s, sizeof(s));
            }
            printf("Got a line: %s\n", line.c_str());
        }
       /* while (m_receiveBuffer->GetLine(line))
        {
            printf("Line: %s (%d)\n", line.c_str(), line.size());
            if (line.empty())
            {
                m_outputBuffer = "HTTP/1.1 200 Ok\r\nContent-Type: text/html\r\n\r\nHello, World !";
            }
        }*/
    }
}

void SocketEvent::HandleWriteEvent() {
    if (m_fileEvent) {
        m_fileEvent->WriteToBuffer(m_outputBuffer);
    }
    m_outputBuffer->Send(m_fd);
}

bool SocketEvent::IsWritable() const
{
    return m_outputBuffer->GetLength() > 0 ||
        (m_fileEvent && m_fileEvent->HasDataAvailable());
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

    std::vector<ListenerEvent *> listeners;

    struct sockaddr_in sin;

    ServerHost *serverHost = new ServerHost(&eventLoop);

    for (int i = 0; i < 10; ++i) {
        sin.sin_family = AF_INET;
        sin.sin_port = htons(8080 + i);
        sin.sin_addr.s_addr = INADDR_ANY;

        ListenerEvent *serv = ListenerEvent::CreateAndBind(serverHost, (struct sockaddr *) &sin, sizeof(sin), 10);
        eventLoop.RegisterEvent(serv);

        listeners.push_back(serv);
    }

    eventLoop.Run();
    return 0;
}