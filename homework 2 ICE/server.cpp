#include <chat.h>
#include <Ice/Ice.h>
#include <stdexcept>
#include <string>

using namespace std;
using namespace chat;

#define __ADAPTER__NAME__ "ChatAdapter"

// definitiion of classes that were generated

//TODO: memory managment?
RoomList room_list = RoomList();
userList user_list = userList();
Ice::ObjectAdapterPtr adapter;


//TODO: memory managment

class UserI : public User {
  void sendMessage(const string& message, const string& fromWho, const Ice::Current& current) override {}
  void sendPrivateMessage(const string& message, const string& fromWho, const Ice::Current& current) override {}
};

class chatRoomI : public chatRoom {
public:
    userList listUsers(const Ice::Current& current) override { return user_list; }
    void join(const string& nick, const UserPrx& who, const Ice::Current& current) override {}
    void postMessage(const string& message, const string& fromWho, const Ice::Current& current) override {}
    UserPrx getUser(const string& name, const Ice::Current& current) override { return nullptr; }
    void Leave(const string& name, const Ice::Current& current) override {}
};


class chatServerI : public chatServer {
    public:

        RoomList getRooms(const Ice::Current& current) {
            return room_list;
        }

        // https://doc.zeroc.com/ice/3.7/client-side-features/proxies/obtaining-proxies
        // Obtaining a Proxy by Invoking Operations
        chatRoomPrx getRoom(const string& name, const Ice::Current& current) override {
            //TODO: use find_if with lambda
            for(int i = 0; i < room_list.size(); i++) {
                if(room_list.at(i).name == name) {
                    // we know that we have the object -> get it
                    // using given id?

                    //TODO: this is propably wrong
// &adapter->find(Ice::stringToIdentity(name);


                    return adapter->find(Ice::stringToIdentity(name));

                }
            }

            //TODO: throw the excetpion
            // RoomInfo room_info = room_list.at(0);
            // throw exception
            //return (chatRoom*)(&adapter->find(Ice::stringToIdentity(room_list.at(0).name)));
        }

        chatRoomPrx newChatRoom(const string& name, const Ice::Current& current) override {
            // ice_getIdentity
            //TODO: memory managment
            chatRoomPtr* chat_room_servant = new chatRoomI();

            Ice::ObjectPrx object_proxy = adapter->addWithUUID(chat_room_servant);

            string name_of_servant = object_proxy->ice_getAdapterId();
            RoomInfo new_room_info();

            new_room_info.name = name_of_servant;
            new_room_info.activeUsers = 0;

            room_list.push_back(new_room_info);

            return object_proxy;
        }

};

class MyApplication : virtual public Ice::Application {
    public:
        virtual int run(int, char*[]) {
            try {
                //Ice::CommunicatorHolder ich = Ice::CommunicatorHolder(argc, argv);
                adapter = communicator()->createObjectAdapterWithEndpoints("ChatAdapter", "default -p 10000");

                //TODO:  tutaj  stworzymy chatserver ktory odbierzemy
                // po stronie klienta

                //auto servant = make_shared<PrinterI>();
                //adapter->add(servant, Ice::stringToIdentity("SimplePrinter"));
                adapter->activate();
                communicator()->waitForShutdown();

            } catch(const std::exception& e) {
                cerr << e.what() << endl;
                return 1;
                }

            return 0;
        }
};

int main(int argc, char* argv[]) {
    MyApplication app;
    return app.main(argc, argv);
}




// module Ice
// {
//     local interface ObjectAdapter
//     {
//         // ...

//         Object* add(Object servant, Identity id);
//         Object* addWithUUID(Object servant);
//         Object  remove(Identity id);
//         Object  find(Identity id);
//         Object  findByProxy(Object* proxy);

//         // ...
//     }
// }
