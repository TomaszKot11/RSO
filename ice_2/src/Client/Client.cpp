#include "Client.h"

using namespace std;
using namespace chat;
namespace ClientApp {
    void Client::createUser() {
        UserPtr object = new UserImpl(username);
        int port = portsUtil.getRandomPort();
        adapter = iceCommunicator->createObjectAdapterWithEndpoints("User" + username, "default -p " + to_string(port));
        user = UserPrx::uncheckedCast(adapter->addWithUUID(object));
        adapter->activate();
    }

    Client::Client(const string& name) : username(name) {
        try {
            iceCommunicator = Ice::initialize();
            int serverPort = portsUtil.getServerPort();
            Ice::ObjectPrx base = iceCommunicator->stringToProxy("Server:default -p " + to_string(serverPort));
            server = ChatServerPrx::checkedCast(base);
            if (!server)
                throw "Invalid proxy";
        } catch (const Ice::Exception& ex) {
            cerr << ex << endl;
        } catch (const char* msg) {
            cerr << msg << endl;
        }
        createUser();
    }

    Client::~Client() {
        for (auto &room : userRooms) {
            auto roomName = room.name;
            ChatRoomPrx chatRoomPrx = server->getRoom(roomName);
            chatRoomPrx->Leave(username);
        }

        if (iceCommunicator)
            iceCommunicator->destroy();
    }

    void Client::createRoom() const {
        scrollConsole();
        string roomName;
        cout << "Enter name for new room " << endl;
        cin >> roomName;
        cin.ignore(1000, '\n');
        try {
            server->newChatRoom(roomName);
        } catch (const chat::NameAlreadyExists& ex) {
            cerr << "Cannot create. There is a room with that name already." << endl;
        } catch (const Ice::UnknownException& ex) {
            cerr << "Cannot create" << endl;
        }
        scrollConsole();
    }

    void Client::printListAllRooms() const {
        scrollConsole();
        auto rooms = server->getRooms();
        cout << "Available rooms are: " << endl;
        for (auto room : rooms) {
            cout << room.name << endl;
        }

        cout << endl;
    }

    void Client::joinToRoom() {
        string name = getNameOfTheRoom();
        try {
            ChatRoomPrx room = server->getRoom(name);
            room->join(username, user);
            RoomInfo roomInfo;
            roomInfo.name = name;
            //TODO: figure this out
            roomInfo.activeUsers = 0;
            userRooms.push_back(roomInfo);
        } catch (const NoSuchRoom& ex) {
            cerr << "There is no such room" << endl;
        } catch (const NickNotAvailable& ex) {
            cerr << "Such userr already exist" << endl;
        } catch (const Ice::UnknownException& ex) {
            cerr << "Operation couldn't be realized." << endl;
        }

        scrollConsole();
    }

    void Client::printUsersInRoom() const {
        try {
            auto users = getUsersInRoom("");
            scrollConsole();
            cout << "Users available in room " << endl;
            for (auto& user : users) {
                cout << user << endl;
            }
        } catch (const Ice::UnknownException& ex) {
            cerr << "Operation couldn't be realized." << endl;
        }
    }

    UserList Client::getUsersInRoom(const string& room_name = "") const {
        string roomName = room_name.empty() ? getNameOfTheRoom() : room_name;
        try {
            ChatRoomPrx room = server->getRoom(roomName);
            UserList users = room->listUsers();

            return users;
        } catch (const NoSuchRoom& ex) {
            cerr << "There is no such room" << endl;
        } catch (Ice::UnknownException& ex) {
            cerr << ex << endl;
        }
        return UserList();
    }

    string Client::getNameOfTheRoom() const {
        scrollConsole();
        string roomName;
        cout << "Enter the name of the room:" << endl;
        cin >> roomName;
        cin.ignore(1000, '\n');
        return roomName;
    }

    void Client::scrollConsole() const {
        for (unsigned int i = 0; i < 2; ++i) {
            cout << endl;
        }
    }

    void Client::disconnect() const {
        int counter = 0;
        for (auto roomsIterator =  userRooms.begin(); roomsIterator != userRooms.end(); ++roomsIterator) {
            try {
                RoomInfo roomInfo = userRooms.at(counter);

                ChatRoomPrx chatRoom = server->getRoom(roomInfo.name);

                chatRoom->Leave(username);
                cout << "Leaving room " << endl;
                counter++;
            } catch (NoSuchUser& ex) {
                cerr << "Ooopss.. something is wrong. You couldn't be found on user list for that room. Sorry!" << endl;
            } catch (Ice::UnknownException& ex) {
                cerr << ex << endl;
            }
        }
    }

    void Client::leaveRoom() {
        scrollConsole();
        string roomName = getNameOfTheRoom();
        for (auto roomsIterator =  userRooms.begin(); roomsIterator != userRooms.end(); ++roomsIterator) {
             if ((*roomsIterator).name == roomName) {
                try {
                    ChatRoomPrx chatRoom = server->getRoom(roomName);
                    chatRoom->Leave(username);
                    cout << "Leaving room " << roomName << endl;
                    userRooms.erase(roomsIterator);

                    return;
                } catch (NoSuchUser& ex) {
                    cerr << "Ooopss.. something is wrong. You couldn't be found on user list for that room. Sorry!" << endl;
                } catch (Ice::UnknownException& ex) {
                    cerr << ex << endl;
                }
            }
        }
        cerr << "You were not joined to room " << roomName << endl;
    }

    void Client::sendPrivateMessageToUser() const {
        string targetUsername;
        string targetChannel;

        UserList usersAvailable;
        try {
            targetChannel = getNameOfTheRoom();
            usersAvailable = getUsersInRoom(targetChannel);
        } catch (NoSuchRoom& ex) {
            cerr << ex << endl;
            return;
        }
        cout << "Enter the name of the user you want to write to" << endl;
        cin >> targetUsername;
        for(auto& targetUser : usersAvailable) {
            if (targetUser == targetUsername) {
                string message;
                cout << "Enter the content of message you want to send to" << endl;
                cin.ignore(1000, '\n');
                getline(cin, message);
                try {
                    ChatRoomPrx chatRoom = server->getRoom(targetChannel);
                    UserPrx userPrx = chatRoom->getUser(targetUsername);
                    userPrx->sendPrivateMessage(message, username);
                } catch(NoSuchUser& ex) {
                    cout<<"No such user."<<endl;
                } catch(NoSuchRoom& ex) {
                    cerr << "No such room" <<endl;

                    return;
                }
                return;
            }
        }
        cerr << "No such user found. Sorry." << endl;
    }

    void Client::sendMessageToRoom() const {
        scrollConsole();
        string targetRoom = getNameOfTheRoom();
        string targetChannel;

        for (auto roomsIterator =  userRooms.begin(); roomsIterator != userRooms.end(); ++roomsIterator) {
            if ((*roomsIterator).name == targetRoom) {
                try {
                    string content;
                    cout << "Please, enter content of message you want to send to" << endl;
                    getline(cin, content);
                    try {
                        ChatRoomPrx chatRoom = server->getRoom(targetRoom);
                        chatRoom->postMessage(content, username);
                    } catch(NoSuchRoom& ex) {
                        cout <<"No such room"<< endl;
                    }

                    return;
                } catch (NoSuchUser& ex) {
                    cerr << "Ooopss.. something is wrong. You couldn't be found on user list for that room. Sorry!" << endl;
                    return;
                }
            }
        }
        cerr << "You are not joined to room " << targetRoom << endl;
    }
}
