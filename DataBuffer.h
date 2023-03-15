//
// Created by Alexis Lavaud on 15/03/2023.
//

#ifndef WEBSERV_DATABUFFER_H
#define WEBSERV_DATABUFFER_H
#include <cstdlib>
#include <string>

class DataBuffer
{
public:
    DataBuffer(size_t capacity);
    ~DataBuffer();

    int Receive(int fd);

    bool GetLine(std::string &line);

    int Get();

    int Put(int ch);
    int Put(std::string s);

    size_t GetAvailableSpace() const;

private:
    char *m_buffer;
    size_t m_capacity;
    size_t m_readPos;
    size_t m_writePos;
};


#endif //WEBSERV_DATABUFFER_H