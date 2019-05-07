#include "UserImpl.h"

namespace LibsIce {
    string UserImpl::getName(const Ice::Current&) {
        return name;
    }

    void UserImpl::sendMessage(const string& message,
                              const string& fromWho,
                              const ::Ice::Current&) {

        auto currentTime = chrono::system_clock::to_time_t(chrono::system_clock::now());
        cout << ctime(&currentTime) << room << "->" << who->getName() << ":" << message << endl;
    }

    void UserImpl::sendPrivateMessage(const string& message,
                                    const string& fromWho,
                                    const ::Ice::Current&) {
        auto currentTime = chrono::system_clock::to_time_t(chrono::system_clock::now());
        cout << ctime(&currentTime) << who->getName() << ": " << message << endl;
    }
}