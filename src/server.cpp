#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include "server.h"
using namespace std;


void start_server(int port) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    const char* response = "Order received!";

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    {
        cerr << "Socket creation error" << endl;
        return;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (::bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) 
    {
        cerr << "Bind Failed" << endl;
        return;
    }

    if (listen(server_fd, 3) < 0) 
    {
        cerr << "Listen Failed" << endl;
        return;
    }

   cout << "Server listening on port " << port << endl;

    if ((new_socket = accept(server_fd, (struct sockaddr*)&address,(socklen_t*)&addrlen)) < 0) 
    {
        cerr << "Accept Failed" << endl;
        return;
    }

    read(new_socket, buffer, 1024);
    cout << "Received: " << buffer << endl;

    send(new_socket, response, strlen(response), 0);
    cout << "Sent acknowledgment" << endl;

    close(new_socket);
    close(server_fd);
}
