#include "ServerImpl.h"
#include "RoomImpl.h"

namespace LibsIce {

    ServerImpl::ServerImpl() {
        ic = Ice::initialize();
    }

    ServerImpl::~ServerImpl() {
        if (ic) {
            try {
                ic->destroy();
            } catch (const Ice::Exception& e) {
                cerr << e << endl;
            }
        }
    }


    ChatRoomPrx ServerImpl::newChatRoom(const string& name, const ::Ice::Current&) {
        // check if the room is not already present
        for (auto &room : roomList) {
            if (room.name == name) {
                throw NameAlreadyExists();
            }
        }

        //TODO random select room factory
        if (roomFactoryList.empty()) {
            // if empty - create some factory
            ChatRoomFactoryPtr object = new RoomFactoryImpl();
            int port = portsUtil.getRandomPort();
            string name = "One";
            ic = Ice::initialize();
            adapter = ic->createObjectAdapterWithEndpoints("SimpleFactory" + name, "default -p " + to_string(port));
            adapter->add(object, ic->stringToIdentity("SimpleFactory" + name));
            adapter->activate();
            Ice::ObjectPrx base = ic->stringToProxy("SimpleFactory" + name + ":default -p " + to_string(port));
            ChatRoomFactoryPrx roomFactory = ChatRoomFactoryPrx::checkedCast(base);
            roomFactoryList.push_back(roomFactory);

            if(roomFactoryList.empty()) {
              //TODO: throw something else
               throw NameAlreadyExists();
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

    RoomList ServerImpl::getRooms(const ::Ice::Current&) {
        return roomList;
    }

    ChatRoomPrx ServerImpl::getRoom(const string& name, const ::Ice::Current& ) {
        for (auto &room : roomList) {
            if (room.name == name)
                return nameChatDictionary[room.name];
        }

        throw NoSuchRoom();
    }

    void ServerImpl::registerFactory(const ChatRoomFactoryPrx& roomFactory, const ::Ice::Current&) {
        roomFactoryList.push_back(roomFactory);
        cout << "Room factory registred " << endl;
    }

    void ServerImpl::unregisterFactory(const ChatRoomFactoryPrx& roomFactory, const ::Ice::Current&) {
        for (auto registredFactoryIt = roomFactoryList.begin(); registredFactoryIt != roomFactoryList.end(); ) {
            if (*registredFactoryIt == roomFactory) {
                registredFactoryIt = roomFactoryList.erase(registredFactoryIt);
            } else {
                ++registredFactoryIt;
            }
        }
    }
}
