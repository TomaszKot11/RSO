#ifndef SERVER_IMPL_H
    #define SERVER_IMPL_H

    #include <random>
    #include <iterator>
    #include <Ice/Ice.h>
    #include "chat.h"
    #include "UserImpl.h"
    #include "PortsUtil.h"

    using namespace Chat;
    using namespace std;

    namespace LibsIce {
        class ServerImpl : public virtual chatServer {
            public:
                ServerImpl();
                virtual void registerFactory(const ChatRoomFactoryPrx&,
                                                 const ::Ice::Current& = ::Ice::Current()) override;
                virtual void unregisterFactory(const ChatRoomFactoryPrx&,
                                                   const ::Ice::Current& = ::Ice::Current()) override;
                virtual ChatRoomPrx newChatRoom(const string&,
                                           const ::Ice::Current& = ::Ice::Current()) override;
                virtual RoomList getRooms(const ::Ice::Current& = ::Ice::Current()) override;
                virtual ChatRoomPrx getRoom(const string&,
                                         const ::Ice::Current& = ::Ice::Current()) override;
                ~ServerImpl();
            private:
                RoomList roomList;
                RoomFactoryList roomFactoryList;
                PortsUtil portsUtil;
                Ice::CommunicatorPtr ic;
                Ice::ObjectAdapterPtr adapter;
        };
    }
#endif