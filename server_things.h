//
// Created by alaeddine on 10/21/17.
//

#ifndef FLOWOVER_SERVER_THINGS_H
#define FLOWOVER_SERVER_THINGS_H

#include "includes.h"
#include "constants_and_definitions.h"
#include "computing_client.h"

class computing_client;

class server_things {
public :
    static atomic<bool> available_slots[MAX_CLIENTS];
    static computing_client **clients;
    static int server_socket;
    static int connected_clients;
    static std::atomic<bool> running;
};

#endif //FLOWOVER_SERVER_THINGS_H
