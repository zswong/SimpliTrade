#pragma once

#include <string>
#include <netinet/in.h>
using namespace std;

const int BUFFER_SIZE = 1024;
const int DEFAULT_PORT = 8080;

void start_server(int port);

struct Order
{
    int id;
    string type;
    int amount;
    string ticker_sym;
    double price;
};