#ifndef CHAT_SERVER_IMPL_H
    #define CHAT_SERVER_IMPL_H

    #include <random>
    #include <iterator>
    #include <Ice/Ice.h>
    #include "ProjectTypes.h"
    #include "chat.h"
    #include "UserImpl.h"
    #include "PortsUtil.h"

    using namespace chat;
    using namespace std;

    namespace Shared {
        class ChatServerImpl : public virtual ChatServer {
            public:
                ChatServerImpl();
                virtual void registerFactory(const ChatRoomFactoryPrx&,
                                                 const ::Ice::Current& = ::Ice::Current()) override;
                virtual void unregisterFactory(const ChatRoomFactoryPrx&,
                                                   const ::Ice::Current& = ::Ice::Current()) override;
                virtual ChatRoomPrx newChatRoom(const string&,
                                           const ::Ice::Current& = ::Ice::Current()) override;
                virtual RoomList getRooms(const ::Ice::Current& = ::Ice::Current()) override;
                virtual ChatRoomPrx getRoom(const string&,
                                         const ::Ice::Current& = ::Ice::Current()) override;
                ~ChatServerImpl();
            private:
                RoomList roomList;
                mutex roomFactoryListMutex;
                RoomFactoryList roomFactoryList;
                PortsUtil portsUtil;
                Ice::CommunicatorPtr ic;
                NameChatRoomDictionary nameChatDictionary;
                Ice::ObjectAdapterPtr adapter;
        };
    }
#endif
