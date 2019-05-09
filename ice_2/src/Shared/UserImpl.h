#ifndef USER_IMPL_H
    #define USER_IMPL_H

   #include <Ice/Ice.h>
    #include "chat.h"
    #include <chrono>
    #include <ctime>

    using namespace chat;
    using namespace std;

    namespace Shared {
        class UserImpl : public virtual User {
            public:
                UserImpl(const string& n) : name(n){};
                virtual void sendMessage(const string&,
                                         const string&,
                                         const ::Ice::Current& = ::Ice::Current()) override;


                virtual string getName(const Ice::Current&) override;
                virtual void sendPrivateMessage(const string&,
                                                const string&,
                                                const ::Ice::Current& = ::Ice::Current()) override;
            private:
                string name;
        };
    }
#endif
