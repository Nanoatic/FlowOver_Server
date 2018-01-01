//
// Created by nanoatic on 25/09/17.
//

#ifndef FLOWOVER_COMPUTINGCLIENT_H
#define FLOWOVER_COMPUTINGCLIENT_H

#include "includes.h"
#include "constants_and_definitions.h"
#include "message_dispatcher.h"
#include "keep_alive.h"
#include "message_poller.h"
#include "queues_over/sync_queue.h"
#include "queues_over/file_queue.h"

class message_dispatcher;

class message_poller;

class keep_alive;

class computing_client {

public:

    computing_client(int index, int client_socket, sockaddr_in client_address);


    void start_client() {
        thread_b = thread(&computing_client::run, this);

    }


    thread thread_b;
    atomic<bool> running{true};

    int client_socket_;
    sockaddr_in client_address_;
    int index_;

    char ip[INET_ADDRSTRLEN];


#ifdef FILE_QUEUE
    file_queue message_dispatcher_queue;
    file_queue message_poller_queue;
#else
    sync_queue<vector<byte >> message_dispatcher_queue;
    sync_queue<vector<byte >> message_poller_queue;
#endif

    void stop();

    void run();

    void init();

    message_dispatcher *md;
    message_poller *mp;
    keep_alive *ka;

};

#include "maths_over/mat.h"


#endif //FLOWOVER_COMPUTINGCLIENT_H
