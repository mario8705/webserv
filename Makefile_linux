NAME:= webserv

DIR_CGI:= Cgi
DIR_CONF:= Config
DIR_HTTP:= Http
DIR_IO:= IO
DIR_NET:= Net
DIR_RGX:= Regex

FLAGS := -Wall -Wextra -Werror -std=c++98

SRCS:= Cgi/CgiManager.cpp  \
      Config/ConfigProperty.cpp \
      Config/PropertyConsumer.cpp \
      Config/PropertyIterator.cpp \
      Config/Token.cpp \
      Http/CGIRequestHandler.cpp \
      Http/FileRequestHandler.cpp \
      Http/HttpClient.cpp \
      Http/HttpClientHandler.cpp \
      Http/HttpException.cpp \
      Http/HttpProtocolCodec.cpp \
      Http/HttpStatusCode.cpp \
      Http/HttpVersion.cpp \
      Http/Request.cpp \
      Http/Response.cpp \
      Http/URL.cpp \
      IO/BufferChain.cpp \
      IO/BufferEvent.cpp \
      IO/DataBuffer.cpp \
      IO/IOEvent.cpp \
      IO/SelectEventLoop.cpp \
      Network/ListenerEvent.cpp \
      Network/NetworkAddress4.cpp \
      Regex/Matcher.cpp \
      Regex/Pattern.cpp \
      Regex/RegexElement.cpp \
      Regex/RegexException.cpp \
      DirectoryListing.cpp \
      main.cpp \
      MimeDatabase.cpp \
      MountPoint.cpp \
      ServerHost.cpp \
      VirtualHost.cpp \
      Webserv.cpp

 INCS:= Cgi/CgiManager.hpp  \
        Config/ConfigProperty.h \
        Config/PropertyConsumer.h \
        Config/Token.h \
        Config/TokenType.h \
        DirectoryListing.h \
        Http/AsyncRequestHandler.h \
        Http/CGIRequestHandler.h \
        Http/FileRequestHandler.h \
        Http/HttpClient.h \
        Http/HttpClientHandler.h \
        Http/HttpException.h \
        Http/HttpMethod.h \
        Http/HttpProtocolCodec.h \
        Http/HttpStatusCode.h \
        Http/HttpVersion.h \
        Http/Request.h \
        Http/RequestHandler.h \
        Http/Response.h \
        Http/URL.h \
        IO/BufferChain.h \
        IO/BufferEvent.h \
        IO/BufferEventHandler.h \
        IO/DataBuffer.h \
        IO/EventLoop.h \
        IO/IOEvent.h \
        IO/SelectEventLoop.h \
        MimeDatabase.h \
        MountPoint.h \
        Network/ConnectionHandler.h \
        Network/ListenerEvent.h \
        Network/NetworkAddress4.h \
        Regex/Matcher.h \
        Regex/Pattern.h \
        Regex/RegexElement.h \
        Regex/RegexException.h \
        ServerHost.h \
        VirtualHost.h \
        Webserv.h

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

%.o:%.cpp $(INCS)
		c++ $(FLAGS) -c $< -o $@

$(NAME): $(OBJS)
		c++ $(FLAGS) $(OBJS) -o $(NAME)
		@echo "[USAGE] ./webserv [configuration file]"

clean:
	rm $(OBJS)

fclean:
	rm -rf $(OBJS)
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re