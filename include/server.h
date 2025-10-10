#pragma once

#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include "server.h"
#include <vector>
#include <sstream>
#include <algorithm>
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

struct Trade 
{
    int buyId;
    int sellId;
    string ticker;
    int amount;
    double price;
};

