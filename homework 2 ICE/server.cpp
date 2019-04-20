#include <chat.h>
#include <Ice/Ice.h>
#include <stdexcept>

using namespace std;
using namespace chat;

#define __ADAPTER__NAME__ "ChatAdapter"

// definitiion of classes that were generated

class MyApplication : virtual public Ice::Application {
    public:
        virtual int run(int, char*[]) {
            try {
                //Ice::CommunicatorHolder ich = Ice::CommunicatorHolder(argc, argv);
                Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapterWithEndpoints("ChatAdapter", "default -p 10000");
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
