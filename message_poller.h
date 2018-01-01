//
// Created by nanoatic on 07/10/17.
//

#ifndef FLOWOVER_MESSAGE_POLLER_H
#define FLOWOVER_MESSAGE_POLLER_H

#include "includes.h"
#include "computing_client.h"

class computing_client;

class message_poller {
public:
    explicit message_poller(computing_client *f);

    void init();

    void run();

    void stop();

    void start_mp() {
        thread_b = thread(&message_poller::run, this);
    }


    thread thread_b;
    atomic<bool> running{true};
    computing_client *parent;

};


#endif //FLOWOVER_MESSAGE_POLLER_H
