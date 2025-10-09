#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include "server.h"
#include <vector>
#include <sstream>
using namespace std;




void start_server(int port) 
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    vector<Order> orderbook;
    int orderCount = 0;

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
        perror("Bind Failed");
        return;
    }

    if (listen(server_fd, 3) < 0) 
    {
        cerr << "Listen Failed" << endl;
        return;
    }

   cout << "Server listening on port " << port << endl;

   while(1)
   {
        
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address,(socklen_t*)&addrlen)) < 0) 
        {
            cerr << "Accept Failed" << endl;
            return;
        }
        cout << "Client Connected " << port << endl;

        while (1)
        {
            memset(buffer, 0, sizeof(buffer));
            int valread = read(new_socket, buffer, 1024);
            if (valread == 0) // client exited
            {
                break;
            }
            
            buffer[valread] = '\0';
            cout << "Received: " << buffer << endl;

            if (strcmp(buffer, "LIST") == 0) 
            {
                string allOrders;
                for (auto& o : orderbook) // loop each element in orderbook
                    allOrders +=  "\n#" + to_string(o.id) + " " + o.type + " " +
                                to_string(o.amount) + " " + o.ticker_sym + " @ " +
                                to_string(o.price);
                if (allOrders.empty()) allOrders = "No orders yet.\n";
                send(new_socket, allOrders.c_str(), allOrders.size(), 0);
                continue;
            }

            //parse the order
            Order item;
            string at_sym;
            stringstream ss(buffer);   // Create a stringstream object
            ss >> item.type >> item.amount >> item.ticker_sym >> at_sym >> item.price;

            if (ss.fail() || (item.type != "BUY" && item.type != "SELL") || at_sym != "@"||item.amount <= 0 || item.price <= 0 || !isalpha(item.ticker_sym[0]))
            {
                string errmsg = "Invalid order format. Use: BUY 10 GOOG @ 150\n";
                send(new_socket, errmsg.c_str(), errmsg.size(), 0);
                continue;
            }

            item.id = ++orderCount;
            orderbook.push_back(item);

            string confirmation = "✅ Order #" + to_string(item.id) + " stored: " + item.type +
                                  " " + to_string(item.amount) + " " + item.ticker_sym + " @ " +
                                  to_string(item.price) + "\n";
            send(new_socket, confirmation.c_str(), confirmation.size(), 0);

            cout << "Stored Order #" << item.id << endl;

            //send(new_socket, response, strlen(response), 0);
            cout << "Sent acknowledgment" << endl;

        }
        close(new_socket);
        cout << "Client disconnected." << endl;
        break;

   }


    close(server_fd);
}
