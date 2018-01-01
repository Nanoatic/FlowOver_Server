//
// Created by nanoatic on 07/10/17.
//

#ifndef FLOWOVER_MESSAGE_DISPATCHER_H
#define FLOWOVER_MESSAGE_DISPATCHER_H

#include "includes.h"
#include "computing_client.h"
#include "interrupt_exception.h"

class computing_client;

class message_dispatcher {
public:
    explicit message_dispatcher(computing_client *f);

    void init();

    void run();

    void stop();

    void start_md() {
        thread_b = thread(&message_dispatcher::run, this);
    }

    thread thread_b;
    atomic<bool> running{true};
    computing_client *parent;


};


#endif //FLOWOVER_MESSAGE_DISPATCHER_H
