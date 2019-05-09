#include "Factory.h"

namespace RoomFactory {
    void Factory::registerRoomFactory() {
        ChatRoomFactoryPtr object = new Shared::ChatRoomFactoryImpl();
        int port = portsUtil.getRandomPort();
        adapter = iceCommunicator->createObjectAdapterWithEndpoints("RoomFactory", "default -p " + to_string(port));
        roomFactory = ChatRoomFactoryPrx::uncheckedCast(adapter->addWithUUID(object));
        adapter->add(object,Ice::stringToIdentity("RoomFactory"));
        adapter->activate();
        server->registerFactory(roomFactory);
        iceCommunicator->waitForShutdown();
    }

    Factory::Factory() {
        try {
            iceCommunicator = Ice::initialize();
            int serverPort = portsUtil.getServerPort();
            Ice::ObjectPrx base = iceCommunicator->stringToProxy("Server:default -p " + to_string(serverPort));
            server = chatServerPrx::checkedCast(base);
            if (!server) {
                throw "Invalid proxy";
            }
        } catch (const NameAlreadyExists &ex) {
            cerr << ex << endl;
        } catch (const NoSuchRoom& ex) {
            cerr << ex << endl;
        } catch (const NoSuchUser& ex) {
            cerr << "No such user" << endl;
        } catch (const Ice::Exception& ex) {
            cerr << ex << endl;
        } catch (const char* msg) {
            cerr << msg << endl;
        }
    }

    Factory::~Factory() {
        server->unregisterFactory(roomFactory);
    }
}