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
        class ServerImpl : public virtual Server {
            public:
                ServerImpl();

                virtual RoomPrx CreateRoom(const string&,
                                           const ::Ice::Current& = ::Ice::Current()) override;
                virtual RoomList getRooms(const ::Ice::Current& = ::Ice::Current()) override;
                virtual RoomPrx FindRoom(const string&,
                                         const ::Ice::Current& = ::Ice::Current()) override;
                virtual void RegisterRoomFactory(const RoomFactoryPrx&,
                                                 const ::Ice::Current& = ::Ice::Current()) override;
                virtual void UnregisterRoomFactory(const RoomFactoryPrx&,
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