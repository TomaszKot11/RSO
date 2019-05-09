#include "ChatRoomImpl.h"

namespace LibsIce {
    UserList ChatRoomImpl::listUsers(const ::Ice::Current&) {
        return users;
    }

    void ChatRoomImpl::join(const string& nick, const UserPrx& who,  const ::Ice::Current&) {
        for(auto const& value: users) {
          if(value == nick)
            throw NameAlreadyExists();
        }

        users.push_back(nick);
        nickUserDictionary[nick] = who;
    }

    void ChatRoomImpl::Leave(const string& nick, const ::Ice::Current&) {
       // check if exists
       bool userExists = false;
       auto counter = users.begin();

       for(auto usersIterator = users.begin(); usersIterator != users.end(); ++usersIterator) {
          if(*(usersIterator) == nick) {
            userExists = true;
          }
        }

      if(!userExists)
        throw NoSuchUser();

      // remove from the users and map
      users.erase(counter);
      nickUserDictionary.erase(nick);

      cout << "Removed user: " << nick << " from room" << endl;
    }

    UserPrx ChatRoomImpl::getUser(const string& nick, const ::Ice::Current&) {
       // check if exists
       for(auto const& value: users) {
          if(value == nick) {
            return nickUserDictionary[nick];
          }
        }

      throw NoSuchUser();
    }

    void ChatRoomImpl::postMessage(const string& message,
                              const string& fromWho,
                               const ::Ice::Current&) {
       bool fromWhoPresent = false;
        // check if sender is inside the room
        for(auto const& value: users) {
          if(value == fromWho)
           fromWhoPresent = true;
        }

      if(!fromWhoPresent)
        throw NoSuchUser();

      auto it = nickUserDictionary.begin();
      while (it != nickUserDictionary.end()) {
        // Accessing VALUE from element pointed by it.
        it->second ->sendMessage(message, fromWho);
        it++;
      }

    }
}