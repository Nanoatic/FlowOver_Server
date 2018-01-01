//
// Created by nanoatic on 07/10/17.
//

#ifndef FLOWOVER_KEEP_ALIVE_H
#define FLOWOVER_KEEP_ALIVE_H

#include "includes.h"
#include "computing_client.h"
#include "helper_class.h"

class computing_client;

class keep_alive {
public:
    explicit keep_alive(computing_client *f);

    void init();

    void run();

    void stop();

    void start_ka() {
        thread_b = thread(&keep_alive::run, this);
    }

    thread thread_b;
    atomic<bool> running{true};
    computing_client *parent;

};


#endif //FLOWOVER_KEEP_ALIVE_H
