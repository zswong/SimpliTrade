#include <iostream>
using namespace std;
#include "server.h"
#include "client.h"

int main(int argc, char* argv[]) {
    if (argc < 2) 
    {
        cout << "Please Enter: ./SimpliTrade [server|client]" << endl;
        return 1;
    }

    string mode = argv[1];

    if (mode == "server") 
    {
        start_server(8080);

    } else if (mode == "client") 
    {
        start_client("127.0.0.1", 8080);
        
    } else 
    {
        cout << "Invalid mode." << endl;
    }

    return 0;
}
