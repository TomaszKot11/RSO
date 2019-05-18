#ifndef CHAT_ROOM_IMPL_H
    #define CHAT_ROOM_IMPL_H

    #include <Ice/Ice.h>
    #include "ProjectTypes.h"
    #include "PortsUtil.h"
    #include "chat.h"
    #include "ChatRoomFactoryImpl.h"

    using namespace std;
    using namespace chat;

    namespace Shared {
        class ChatRoomImpl : public virtual ChatRoom {
            public:
                ChatRoomImpl(const string &n) : roomName(n) {}
                virtual UserList listUsers(const ::Ice::Current& = ::Ice::Current()) override;

                virtual void join(const string&,
                          const UserPrx&,
                            const ::Ice::Current& = ::Ice::Current()) override;

                virtual UserPrx getUser(const string&, const ::Ice::Current& = ::Ice::Current()) override;

                virtual void postMessage(const string&,
                                         const string&,
                                         const ::Ice::Current& = ::Ice::Current()) override;
                virtual void Leave( const string&,
                                const ::Ice::Current& = ::Ice::Current()) override;

              private:
                string roomName;
                UserList users;
                //::std::map< ::std::string, UserPrx> NickUserDictinary;
                NickUserDictinary nickUserDictionary;
        };
    }

#endif
