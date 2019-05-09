CC = g++
CFLAGS = -g #-Wall -Werror -Wpedantic -Wextra
LIBSFLAGS = -pthread -lIce -std=c++11 # -lIceUtil
TARGETDIR = bin/
TARGETCLIENT = bin/client
TARGETSERVER = bin/server
TARGETFACTORY = bin/factory
CLIENTSRCDIR = src/Client
SERVERSRCDIR = src/Server
FACTORYSRCDIR = src/RoomFactory
SHARED = src/Shared
ICEDIR = ice

INCLUDES = -I $(CLIENTSRCDIR) -I $(ICEDIR) -I $(SHARED) -I $(SERVERSRCDIR) -I $(FACTORYSRCDIR)
SRCICE = $(ICEDIR)/chat.cpp $(SHARED)/UserImpl.cpp $(SHARED)/ChatServerImpl.cpp $(SHARED)/ChatRoomFactoryImpl.cpp $(SHARED)/ChatRoomImpl.cpp
SRCSCLIENT = $(SRCICE) $(CLIENTSRCDIR)/Client.cpp  $(CLIENTSRCDIR)/client_runner.cpp $(CLIENTSRCDIR)/Menu.cpp
SRCSSERVER = $(SRCICE) $(SERVERSRCDIR)/server_runner.cpp
SRCFACTORY = $(SRCICE) $(FACTORYSRCDIR)/main.cpp $(FACTORYSRCDIR)/Factory.cpp
SRC = $(SRCSCLIENT) $(SRCSSERVER)

OBJS = $(SRC:.cpp=.o)
.PHONY: clean All

All: $(TARGETCLIENT) $(TARGETSERVER) $(TARGETFACTORY)
        @echo "----------Building project:[ Zadanie 2 Chat - bin ]----------"

$(TARGETCLIENT):
		mkdir -p $(TARGETDIR)
		$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGETCLIENT) $(SRCSCLIENT) $(LIBSFLAGS)


$(TARGETSERVER):
		mkdir -p $(TARGETDIR)
		$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGETSERVER) $(SRCSSERVER) $(LIBSFLAGS)

$(TARGETFACTORY):
		mkdir -p $(TARGETDIR)
		$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGETFACTORY) $(SRCFACTORY) $(LIBSFLAGS)

.cpp.o:
		$(CC) $(CFLAGS) $(INCLUDES) -o $< -o $@

clean:
		@echo "----------Cleaning project:[ Zadanie 2 Chat - bin ]----------"
		$(RM) *.o *~ $(TARGETCLIENT)
		$(RM) *.o *~ $(TARGETSERVER)
		$(RM) *.o *~ $(TARGETFACTORY)
