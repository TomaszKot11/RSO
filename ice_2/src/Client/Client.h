#ifndef CLIENT_H
    #define CLIENT_H

    #include <thread>
    #include <Ice/Ice.h>
    #include "chat.h"
    #include "PortsUtil.h"
    #include "UserImpl.h"

    using namespace chat;
    using namespace std;
    using namespace LibsIce;

  namespace ClientApp {
      class Client {
        public:
          Client(const string&);
          void createRoom() const;
          void printListAllRooms() const;
          void joinToRoom();
          void printUsersInRoom() const;
          void leaveRoom();
          void sendPrivateMessageToUser() const;
          void sendMessageToRoom() const;
          void disconnect() const;
          ~Client();

          private:
            string username;
            UserPrx user;
            chatServerPrx server;
            Ice::CommunicatorPtr iceCommunicator;
            Ice::ObjectAdapterPtr adapter;
            PortsUtil portsUtil;
            RoomList userRooms;

            void createUser();
            string getNameOfTheRoom() const;
            void scrollConsole() const;
            UserList getUsersInRoom(const string&) const;
      };
  }
#endif
