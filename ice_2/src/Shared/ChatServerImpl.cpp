#include "ChatServerImpl.h"
#include "ChatRoomImpl.h"

namespace Shared {

    ChatServerImpl::ChatServerImpl() {
        ic = Ice::initialize();
    }

    ChatServerImpl::~ChatServerImpl() {
        if (ic) {
            try {
                ic->destroy();
            } catch (const Ice::Exception& e) {
                cerr << e << endl;
            }
        }
    }


    ChatRoomPrx ChatServerImpl::newChatRoom(const string& name, const ::Ice::Current&) {
        // check if the room is not already present
        for (auto &room : roomList) {
            if (room.name == name) {
                throw NameAlreadyExists();
            }
        }

        if (roomFactoryList.empty()) {
            // if empty - create some factory
            ChatRoomFactoryPtr object = new ChatRoomFactoryImpl();
            int port = portsUtil.getRandomPort();
            string name = "One";
            ic = Ice::initialize();
            adapter = ic->createObjectAdapterWithEndpoints("SimpleFactory" + name, "default -p " + to_string(port));
            adapter->add(object, Ice::stringToIdentity("SimpleFactory" + name));
            adapter->activate();
            Ice::ObjectPrx base = ic->stringToProxy("SimpleFactory" + name + ":default -p " + to_string(port));
            ChatRoomFactoryPrx roomFactory = ChatRoomFactoryPrx::checkedCast(base);
            roomFactoryList.push_back(roomFactory);

            if(roomFactoryList.empty()) {
              throw "There is no room factories.";
            }
        }

        ChatRoomFactoryPrx roomFactory = roomFactoryList.back();
        ChatRoomPrx room = roomFactory->newChatRoom(name);
        cout << "Creating room with name: " << name << endl;
        RoomInfo roomInfo;
        roomInfo.name = name;
        roomInfo.activeUsers = 0;
        roomList.push_back(roomInfo);
        nameChatDictionary[name] = room;
        cout << "Room " << name << " created." << endl;

        return room;
    }

    RoomList ChatServerImpl::getRooms(const ::Ice::Current&) {
        return roomList;
    }

    ChatRoomPrx ChatServerImpl::getRoom(const string& name, const ::Ice::Current& ) {
        for (auto &room : roomList) {
            cout<<"Room name: " << room.name<<" room given: " << name << endl;
            if (room.name == name) {
                cout<<"Returning chat room proxy!"<<endl;
                return nameChatDictionary[room.name];
            }
        }

        throw NoSuchRoom();
    }

    void ChatServerImpl::registerFactory(const ChatRoomFactoryPrx& roomFactory, const ::Ice::Current&) {
        //TODO: add mutex
        roomFactoryList.push_back(roomFactory);
        cout << "Room factory registred " << endl;
    }

    void ChatServerImpl::unregisterFactory(const ChatRoomFactoryPrx& roomFactory, const ::Ice::Current&) {
        // TODO: mutex
        for (auto registredFactoryIt = roomFactoryList.begin(); registredFactoryIt != roomFactoryList.end(); ) {
            if (*registredFactoryIt == roomFactory) {
                registredFactoryIt = roomFactoryList.erase(registredFactoryIt);
            } else {
                ++registredFactoryIt;
            }
        }
    }
}
