#ifndef ROOM_FACTORY_IMPL_H
    #define ROOM_FACTORY_IMPL_H

    #include <Ice/Ice.h>
    #include "chat.h"
    #include "RoomImpl.h"
    #include "PortsUtil.h"

    using namespace std;
    using namespace chat;

    namespace LibsIce {
        class RoomFactoryImpl : public virtual ChatRoomFactory {
            public:
                RoomFactoryImpl();
                virtual ChatRoomPrx newChatRoom(const string&,
                                           const ::Ice::Current& = ::Ice::Current()) override;
                ~RoomFactoryImpl();
            private:
                RoomList roomList;
                Ice::CommunicatorPtr ic;
                Ice::ObjectAdapterPtr adapter;
                PortsUtil portsUtil;
        };
    }

#endif