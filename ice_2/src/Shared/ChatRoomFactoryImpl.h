#ifndef CHAT_ROOM_FACTORY_IMPL_H
    #define CHAT_ROOM_FACTORY_IMPL_H

    #include <Ice/Ice.h>
    #include "chat.h"
    #include "ChatRoomImpl.h"
    #include "PortsUtil.h"

    using namespace std;
    using namespace chat;

    namespace Shared {
        class ChatRoomFactoryImpl : public virtual ChatRoomFactory {
            public:
                ChatRoomFactoryImpl();
                virtual ChatRoomPrx newChatRoom(const string&,
                                           const ::Ice::Current& = ::Ice::Current()) override;
                virtual int getNumberOfRooms(const ::Ice::Current& = ::Ice::Current()) override;
                ~ChatRoomFactoryImpl();
            private:
                RoomList roomList;
                Ice::CommunicatorPtr ic;
                Ice::ObjectAdapterPtr adapter;
                PortsUtil portsUtil;
        };
    }

#endif
