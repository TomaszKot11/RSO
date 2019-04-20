module chat
{

struct RoomInfo
{
   string name;
   short activeUsers;
};

sequence<RoomInfo> RoomList;

exception NoSuchRoom {};
exception NameAlreadyExists {};

exception NoSuchUser {};

sequence<string> userList;

exception NickNotAvailable {};

interface User {
  void sendMessage(string message, string fromWho); 
  void sendPrivateMessage(string message, string fromWho); 
};

interface chatRoom {
    userList listUsers();
    void join(string nick, User* who) throws NickNotAvailable;
    void postMessage(string message, string fromWho);
    User* getUser(string name) throws NoSuchUser;
    void Leave(string name);
};

interface chatRoomFactory {
  chatRoom* newChatRoom(string name);
};

interface chatServer {
  RoomList getRooms();
  chatRoom* getRoom(string name) throws NoSuchRoom;
  chatRoom* newChatRoom(string name) throws NameAlreadyExists;
  void registerFactory(chatRoomFactory* crf);
  void unregisterFactory(chatRoomFactory* crf);
};

};
