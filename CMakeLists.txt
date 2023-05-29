cmake_minimum_required(VERSION 3.24)
project(webserv)

set(CMAKE_CXX_STANDARD 98)

add_executable(webserv
        ./IO/IOEvent.cpp
        ./IO/SelectEventLoop.cpp
        ./IO/DataBuffer.cpp
        ./IO/BufferEvent.cpp
        ./IO/BufferChain.cpp
        ./main.cpp
        ./Cgi/CgiManager.hpp
        ./Cgi/CgiManager.cpp
        ./ServerHost.cpp
        ./Regex/RegexElement.cpp
        ./Regex/RegexException.cpp
        ./Regex/Pattern.cpp
        ./Regex/Matcher.cpp
        ./MimeDatabase.cpp
        ./Http/HttpException.cpp
        ./Http/HttpClientHandler.cpp
        ./Http/URL.cpp
        ./Http/Response.cpp
        ./Http/HttpVersion.cpp
        ./Http/FileRequestHandler.cpp
        ./Http/HttpProtocolCodec.cpp
        ./Http/Request.cpp
        ./Http/HttpStatusCode.cpp
        ./Http/HttpClient.cpp
        ./Webserv.cpp
        ./Config/PropertyConsumer.cpp
        ./Config/Token.cpp
        ./Config/ConfigProperty.cpp
        ./VirtualHost.cpp
        ./string_utils.hpp
        ./MountPoint.cpp
        ./Network/NetworkAddress4.cpp
        ./Network/ListenerEvent.cpp Config/PropertyIterator.cpp Http/CGIRequestHandler.cpp Http/CGIRequestHandler.h)
