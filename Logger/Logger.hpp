//
// Created by sansho on 21/03/23.
//

#ifndef WEBSERV_LOGGER_HPP
#define WEBSERV_LOGGER_HPP

#include <iostream>
#include <ctime>
#include <fstream>
#include <sstream>

enum AnsiColor
{
    kAnsiColor_Black = 0,
    kAnsiColor_Red,
    kAnsiColor_Green,
    kAnsiColor_Yellow,
    kAnsiColor_Blue,
    kAnsiColor_Magenta,
    kAnsiColor_Cyan,
    kAnsiColor_Reset,
};

enum LogLevel
{
    kLogLevel_Debug,
    kLogLevel_Info,
    kLogLevel_Warning,
    kLogLevel_Error,
};

class ILogger;

class LoggerFacility : public std::ostream
{
    friend class ILogger;

public:
    LoggerFacility(std::streambuf *buf, LogLevel level, bool ansiColors)
        : std::ostream(buf), m_level(level), m_ansiColors(ansiColors)
    {
    }

    ~LoggerFacility()
    {
    }

    template <typename T>
    LoggerFacility &operator<<(const T &value)
    {
        return *this;
    }

    bool HasColors() const
    {
        return m_ansiColors;
    }

    LoggerFacility &PutColor(const AnsiColor &color)
    {
        if (m_ansiColors) {
            *this << "\x1b[3" << (char) ('0' + (int) color) << "m";
        }
        return *this;
    }

private:
    LoggerFacility &Begin()
    {
        PutColor(kAnsiColor_Blue);
        *this << "[LEVEL HERE] ";
        return *this;
    }

    LogLevel m_level;
    bool m_ansiColors;
};

inline std::ostream &operator<<(std::ostream &os, const AnsiColor &color)
{
    LoggerFacility *f;

    if ((f = dynamic_cast<LoggerFacility *>(&os)))
    {
        return f->PutColor(color);
    }
    return os;
}

class ILogger
{
public:
	virtual ~ILogger();

    virtual std::ostream &Log(LogLevel level);

protected:
    virtual LoggerFacility &GetFacility(LogLevel level) = 0;
};


#endif //WEBSERV_LOGGER_HPP
