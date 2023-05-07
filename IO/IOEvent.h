//
// Created by alavaud on 3/13/23.
//

#ifndef WEBSERV_IOEVENT_H
#define WEBSERV_IOEVENT_H

/**
 * The base class for receiving I/O events from the event loop.
 */
class IOEvent
{
public:
    virtual ~IOEvent();

    /**
     * Invoked when IsReadable() returns true and data available in the file descriptor.
     */
    virtual void NotifyRead() = 0;

    /**
     * Invoked when IsWritable() returns true and at least one byte can be written into the file descriptor.
     */
    virtual void NotifyWrite() = 0;

    /**
     * Adds the file descriptor to the read set of the event loop thus allowing NotifyRead to be invoked in case of
     * available data.
     * @return true if we accept incoming data
     */
    virtual bool IsReadable() const = 0;

    /**
     * Adds the file descriptor to the write set of the event loop thus allowing NotifyWrite to be invoked in case of
     * sufficient space in the write buffer.
     * @return true if we want to be notified if the file descriptor is writable
     */
    virtual bool IsWritable() const = 0;

    /**
     * Simply returns the file descriptor to be monitored by the event loop.
     * @return the file descriptor to be monitored
     */
    virtual int GetFileDescriptor() const;

protected:
    explicit IOEvent(int fd);

    int m_fd;
};

#endif //WEBSERV_IOEVENT_H
