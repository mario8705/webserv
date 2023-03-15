cmake_minimum_required(VERSION 3.24)
project(webserv)

set(CMAKE_CXX_STANDARD 98)

add_executable(webserv main.cpp CommandLineOpt.cpp CommandLineOpt.h EventLoop.h IOEvent.h IOEventBase.cpp IOEventBase.h SelectEventLoop.cpp SelectEventLoop.h ListenerEvent.cpp ListenerEvent.h ConnectionHandler.h)
