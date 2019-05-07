#include "RoomImpl.h"

namespace LibsIce {
    UserList RoomImpl::listUsers(const ::Ice::Current&) {
        return users;
    }

    void RoomImpl::join(const string& nick, const UserPrx& who,  const ::Ice::Current&) {
        for(auto const& value: users) {
          if(value == nick)
            throw NameAlreadyExists();
        }

        users.push_back(nick);
        nickUserDictionary[nick] = who;
    }

    void RoomImpl::Leave(const string& nick, const ::Ice::Current&) {
       // check if exists
       bool userExists = false;
       auto counter = users.begin();

       for(auto const& value: users) {
          if(value == nick) {
            userExists = true;
          }
          counter++;
        }

      if(!userExists)
        throw NoSuchUser();

      // remove from the users and map
      users.erase(counter);
      nickUserDictionary.erase(nick);

      cout << "Removed user: " << nick << " from room" << endl;
    }

    UserPrx RoomImpl::getUser(const string& nick, const ::Ice::Current&) {
       // check if exists
       for(auto const& value: users) {
          if(value == nick)
            throw NameAlreadyExists();
        }

      return nickUserDictionary[nick];
    }

    void RoomImpl::postMessage(const string& message,
                              const string& fromWho,
                               const ::Ice::Current&) {
        // check if sender is inside the room
         for(auto const& value: users) {
          if(value == fromWho)
            throw NoSuchUser();
        }

      auto it = nickUserDictionary.begin();
      while (it != nickUserDictionary.end()) {
      // Accessing VALUE from element pointed by it.
        it->second ->sendMessage(message, fromWho);
        it++;
      }

    }
}