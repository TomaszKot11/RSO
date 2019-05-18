#ifndef PROJECT_TYPES_H
    #define PROJECT_TYPES_H

    #include "chat.h"

    using namespace std;
    using namespace chat;

    typedef ::std::map< ::std::string, ChatRoomPrx> NameChatRoomDictionary;
    typedef ::std::vector<ChatRoomFactoryPrx> RoomFactoryList;
    typedef ::std::map< ::std::string, UserPrx> NickUserDictinary;
#endif
