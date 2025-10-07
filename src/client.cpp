#include <iostream>
#include "client.h"
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
using namespace std;
#include <string>

void start_client(const char* server_ip, int port)
{
    //file descriptor for socket
    int client_socket = 0;
    sockaddr_in serv_addr;

    string message;
    cout << "Enter order: ";
    getline(cin, message);

    char buffer[1024] = {0};

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        cerr << "Socket creation error" << endl;
        return;
    }

    //server address structure
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port); //convert host byte order to network byte order
    inet_pton(AF_INET, server_ip, &serv_addr.sin_addr);

    //connect to server 
    if (connect(client_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
    {
        cerr << "Connection Failed" << endl;
        return;
    }

    send(client_socket, message.c_str(), message.size(), 0);
    cout << "Sent: " << message << endl;

    read(client_socket, buffer, 1024);
    cout << "Server: " << buffer << endl;
    
    close(client_socket);
}

