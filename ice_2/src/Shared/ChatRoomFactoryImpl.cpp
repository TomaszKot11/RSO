#include "ChatRoomFactoryImpl.h"

namespace Shared {
    ChatRoomPrx ChatRoomFactoryImpl::newChatRoom(const string& name, const ::Ice::Current&) {
        ChatRoomPtr object = new ChatRoomImpl(name);
        cout << "RoomFactory::Creating room " << name << endl;
        int port = portsUtil.getRandomPort();

        adapter = ic->createObjectAdapterWithEndpoints("SimpleRoom" + name, "default -p " + to_string(port));
        adapter->add(object, Ice::stringToIdentity("SimpleRoom" + name));
        adapter->activate();
        Ice::ObjectPrx base = ic->stringToProxy("SimpleRoom" + name + ":default -p " + to_string(port));
        ChatRoomPrx room = ChatRoomPrx::checkedCast(base);
        cout << "RoomFactory::Room created" << endl;

        return room;
    }

    ChatRoomFactoryImpl::ChatRoomFactoryImpl() {
        ic = Ice::initialize();
    }

    int ChatRoomFactoryImpl::getNumberOfRooms(const ::Ice::Current&) {
        return roomList.size();
    }

    ChatRoomFactoryImpl::~ChatRoomFactoryImpl() {
        if (ic) {
            try {
                ic->destroy();
            } catch (const Ice::Exception& e) {
                cerr << e << endl;
            }
        }
    }
}
