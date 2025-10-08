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

    cout << "Connected to Server at " <<server_ip<< ": " << port;
    string order;
    while (1)
    {
        cout << "\nEnter order (or 'exit' to quit) : ";
        getline(cin, order);

        if (order == "exit" | order == "Exit")
        {
            break;
        }

        send(client_socket, order.c_str(), order.size(), 0);
        cout << "Sent: " << order << endl;

        memset(buffer, 0, sizeof(buffer)); //clear buffer
        read(client_socket, buffer, 1024);
        cout << "Server: " << buffer << endl;

    }
    
    
    close(client_socket);
}

