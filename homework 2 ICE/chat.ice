module chat
{

struct RoomInfo
{
   string name;
   short activeUsers;
};

sequence<RoomInfo> RoomList;

//exception NoSuchRoom {};
//exception NameAlreadyExists {};

//exception NoSuchUser {};

sequence<string> userList;

//exception NickNotAvailable {};

interface User {
  void sendMessage(string message, string fromWho);
  void sendPrivateMessage(string message, string fromWho);
};

interface chatRoom {
    userList listUsers();
    void join(string nick, User* who);
    void postMessage(string message, string fromWho);
    User* getUser(string name);
    void Leave(string name);
};

interface chatRoomFactory {
  chatRoom* newChatRoom(string name);
};

interface chatServer {
  RoomList getRooms(); // implemented
  chatRoom* getRoom(string name); // almost implemented
  chatRoom* newChatRoom(string name); // almost implemented
  void registerFactory(chatRoomFactory* crf); // czy to ma byc zarejestrowane jako
  // servant? i wukorzystywać to w newChatRoom?
  void unregisterFactory(chatRoomFactory* crf);
};

};
