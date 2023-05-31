//
// Created by sansho on 21/03/23.
//

#include "Logger.hpp"

ILogger::~ILogger()
{
}

std::ostream &ILogger::Log(LogLevel level)
{
    return GetFacility(level).Begin();
}
