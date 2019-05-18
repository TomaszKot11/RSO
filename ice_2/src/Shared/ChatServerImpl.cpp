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

        if(roomFactoryList.empty()) {
            throw "There is no room factories.";
        }
           // std::min_element(roomFactoryList.begin(), roomFactoryList.end(), [] (ChatRoomFactoryPrx lhs, ChatRoomFactoryPrx rhs) {return lhs->getNumberOfRooms() < rhs->getNumberOfRooms(); });
        cout<<"Factory number is: "<< roomFactoryList.size() <<endl;
        int index = 0, min_index = 0, min = 0;
        for(; index < roomFactoryList.size(); index++) {
            if(min < roomFactoryList[index]->getNumberOfRooms())
                min_index = index;
        }


        ChatRoomFactoryPrx roomFactory = roomFactoryList[min_index];
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
