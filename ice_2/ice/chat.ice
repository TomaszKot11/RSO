module chat
{

struct RoomInfo
{
   string name;
   short activeUsers;
};

exception NoSuchRoom {};
exception NameAlreadyExists {};
exception NoSuchUser {};
exception NickNotAvailable {};

sequence<RoomInfo> RoomList;
sequence<string> UserList;

interface User {
  void sendMessage(string message, string fromWho);
  void sendPrivateMessage(string message, string fromWho);
   // added
  string getName();
};

// added
dictionary<string, User*> NickUserDictinary;

interface ChatRoom {
    UserList listUsers();
    void join(string nick, User* who) throws NickNotAvailable;
    void postMessage(string message, string fromWho);
    User* getUser(string name) throws NoSuchUser;
    void Leave(string name);
};

interface ChatRoomFactory {
  ChatRoom* newChatRoom(string name);
};

// added
sequence<ChatRoomFactory*> RoomFactoryList;

interface chatServer {
  RoomList getRooms();
  ChatRoom* getRoom(string name) throws NoSuchRoom;
  ChatRoom* newChatRoom(string name) throws NameAlreadyExists;
  void registerFactory(ChatRoomFactory* crf);
  void unregisterFactory(ChatRoomFactory* crf);
};

};
