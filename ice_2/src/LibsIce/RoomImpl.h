#ifndef ROOM_IMPL_H
    #define ROOM_IMPL_H

    #include "chat.h"
    #include <Ice/Ice.h>
    #include "PortsUtil.h"
    #include "RoomFactoryImpl.h"

    using namespace std;
    using namespace Chat;

    namespace LibsIce {
        class RoomImpl : public virtual ChatRoom {
            public:
                RoomImpl(const string &n) : roomName(n) {}
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
                UsernamesWithPasswordList usernamesWithPasswords;
        };
    }

#endif
