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


    RoomPrx ServerImpl::CreateRoom(const string& name, const ::Ice::Current&) {
        for (auto &room : roomList) {
            if (room->getName() == name) {
                throw RoomAlreadyExist();
            }
        }
        //TODO random select room factory
        if (roomFactoryList.empty()) {
            // if empty - create some factory
            RoomFactoryPtr object = new RoomFactoryImpl();
            int port = portsUtil.getRandomPort();
            string name = "One";
            ic = Ice::initialize();
            adapter = ic->createObjectAdapterWithEndpoints("SimpleFactory" + name, "default -p " + to_string(port));
            adapter->add(object, ic->stringToIdentity("SimpleFactory" + name));
            adapter->activate();
            Ice::ObjectPrx base = ic->stringToProxy("SimpleFactory" + name + ":default -p " + to_string(port));
            RoomFactoryPrx roomFactory = RoomFactoryPrx::checkedCast(base);
            roomFactoryList.push_back(roomFactory);
            if(roomFactoryList.empty()) {
               throw NoResourcesAvailable();
            }
        }

        RoomFactoryPrx roomFactory = roomFactoryList.back();
        RoomPrx room = roomFactory->createRoom(name);
        cout << "Creating room with name: " << name << endl;
        roomList.push_back(room);
        cout << "Room " << name << " created." << endl;
        return room;
    }

    RoomList ServerImpl::getRooms(const ::Ice::Current&) {
        return roomList;
    }

    RoomPrx ServerImpl::FindRoom(const string& name, const ::Ice::Current& ) {
        for (auto &room : roomList) {
            if (room->getName() == name) {
                return room;
            }
        }
        throw NoSuchRoomExist();
    }

    void ServerImpl::RegisterRoomFactory(const RoomFactoryPrx& roomFactory, const ::Ice::Current&) {
        roomFactoryList.push_back(roomFactory);
        cout << "Room factory registred " << endl;
    }

    void ServerImpl::UnregisterRoomFactory(const RoomFactoryPrx& roomFactory, const ::Ice::Current&) {
        for (auto registredFactoryIt = roomFactoryList.begin(); registredFactoryIt != roomFactoryList.end(); ) {
            if (*registredFactoryIt == roomFactory) {
                registredFactoryIt = roomFactoryList.erase(registredFactoryIt);
            } else {
                ++registredFactoryIt;
            }
        }
    }
}
