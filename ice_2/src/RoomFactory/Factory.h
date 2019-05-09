#ifndef FACTORY_H
    #define FACTORY_H

    #include <Ice/Ice.h>
    #include "chat.h"
    #include "ChatRoomFactoryImpl.h"
    #include "PortsUtil.h"

    using namespace std;
    using namespace chat;
    using namespace Shared;

    namespace RoomFactory {
        class Factory {
            public:
                Factory();
                void registerRoomFactory();
                ~Factory();
            private:
                ChatServerPrx server;
                Ice::CommunicatorPtr iceCommunicator;
                Ice::ObjectAdapterPtr adapter;
                ChatRoomFactoryPrx roomFactory;
                PortsUtil portsUtil;
        };
    }

#endif