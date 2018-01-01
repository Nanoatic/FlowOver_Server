//
// Created by alaeddine on 10/21/17.
//

#include "server_things.h"

atomic<bool> server_things::available_slots[MAX_CLIENTS]{true};
computing_client **server_things::clients = new computing_client *[MAX_CLIENTS];
int server_things::server_socket = -1;
int server_things::connected_clients = 0;
std::atomic<bool> server_things::running = true;