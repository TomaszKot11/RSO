#include "UserImpl.h"

namespace Shared {
    void UserImpl::sendMessage(const string& message,
                              const string& fromWho,
                              const ::Ice::Current&) {

        auto currentTime = chrono::system_clock::to_time_t(chrono::system_clock::now());
        cout << ctime(&currentTime) << fromWho << " says: " << message << endl;
    }

    void UserImpl::sendPrivateMessage(const string& message,
                                    const string& fromWho,
                                    const ::Ice::Current&) {

        auto currentTime = chrono::system_clock::to_time_t(chrono::system_clock::now());
        cout << ctime(&currentTime) << fromWho << " says: "<< message << endl;
    }
}