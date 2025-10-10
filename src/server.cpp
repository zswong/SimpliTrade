#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include "server.h"
#include <vector>
#include <sstream>
#include <algorithm>
using namespace std;


void matchOrders(Order &incoming, vector<Order> &oppositeBook, vector<Order> &sameBook, int id, int client_socket)
{
    stringstream tradeLog;

    
    if (incoming.type == "BUY")//sort SELL to buy lowest price (acending)
        sort(oppositeBook.begin(), oppositeBook.end(),
         [](Order &a, Order &b) { return a.price < b.price; }); add
    else //sort BUY to sell to highest bidder (decending)
        sort(oppositeBook.begin(), oppositeBook.end(),
         [](Order &a, Order &b) { return a.price > b.price; });

    for (auto it = oppositeBook.begin(); it != oppositeBook.end() && incoming.amount > 0;)
    {
        bool match = false;

        if (incoming.type == "BUY" && incoming.price >= it->price)//buy price is higher than or equal to sell price
            match = true;
        else if (incoming.type == "SELL" && incoming.price <= it->price)// sell price is lower than or equal buy price
            match = true;

        if (match)//found a match to trade 
        {
            int tradedAmount = min(incoming.amount, it->amount); // can only trade the min amount 
            double tradePrice = it->price; // pay the price the seller posted or sell the price the buyer posted

            tradeLog << "💸 TRADE: " << tradedAmount << " " << it->ticker_sym
                     << " @ " << tradePrice << " between Order#" << id
                     << " and Order#" << it->id << "\n";

            incoming.amount -= tradedAmount;
            it->amount -= tradedAmount;

            if (it->amount == 0) // no more -> remove from orders
                it = oppositeBook.erase(it);
            else
                ++it;
        }
        else
        {
            ++it; // No match, move to next
        }
    }

    if (incoming.amount > 0)
        sameBook.push_back(incoming);

    string msg = tradeLog.str();
    if (msg.empty()) msg = "No trades executed.\n";
    send(client_socket, msg.c_str(), msg.size(), 0);
}


void start_server(int port) 
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    vector<Order> orderbook;
    vector<Order> buyOrders;
    vector<Order> sellOrders;
   
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
                string allOrders = "\nBUY Orders:\n";
                for (auto& o : buyOrders)
                    allOrders +=  "\n#" + to_string(o.id) + " " + o.type + " " +
                                to_string(o.amount) + " " + o.ticker_sym + " @ " +
                                to_string(o.price);

                allOrders += "\nSELL Orders:\n";
                for (auto& o : sellOrders)
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

            if (ss.fail() || (item.type != "BUY" && item.type != "SELL") || at_sym != "@"||item.amount <= 0 || item.price <= 0)
            {
                string errmsg = "Invalid order format. Use: BUY 10 GOOG @ 150\n";
                send(new_socket, errmsg.c_str(), errmsg.size(), 0);
                continue;
            }

            if (item.ticker_sym.empty() || !all_of(item.ticker_sym.begin(), item.ticker_sym.end(), ::isalpha)) 
            {
                string errmsg = "Invalid ticker symbol. Use only letters.\n";
                send(new_socket, errmsg.c_str(), errmsg.size(), 0);
                continue;
            }


            item.id = ++orderCount;
            
            
            if (item.type == "BUY") 
            {
                matchOrders(item, sellOrders, buyOrders, item.id, new_socket);
            } 
            else if (item.type == "SELL") 
            {
                matchOrders(item, buyOrders, sellOrders, item.id, new_socket);
            }

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
