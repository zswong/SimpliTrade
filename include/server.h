#pragma once

#include <string>
#include <netinet/in.h>

const int BUFFER_SIZE = 1024;
const int DEFAULT_PORT = 8080;

void start_server(int port);