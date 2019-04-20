#include <chat.h>
#include <Ice/Ice.h>
#include <stdexcept>

#define __ADAPTER__NAME__ "ChatAdapter"

using namespace std;
using namespace chat;

class MyApplication : virtual public Ice::Application {
    public:
        virtual int run(int, char*[]) {
            try {
                //Ice::CommunicatorHolder ich = Ice::CommunicatorHolder(argc, argv);
                Ice::ObjectPrx base = communicator()->stringToProxy("ChatAdapter:default -p 10000");
                //auto printer = Ice::checkedCast<PrinterPrx>(base);
                //if(!printer)
                //{
                  //  throw std::runtime_error("Invalid proxy");
                //}

                //printer->printString("Hello World!");
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