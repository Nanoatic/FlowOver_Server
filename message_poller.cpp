//
// Created by nanoatic on 07/10/17.
//

#include "message_poller.h"

message_poller::message_poller(computing_client *f) : parent(f) {

}

void message_poller::init() {


    helper_class::display_info_syncronized_ln("Poller started with id " + std::to_string(parent->index_));

}

void message_poller::run() {
    init();
    while (running) {
        unsigned short size;
        helper_class::ReadXBytes(parent->client_socket_, 2, &size);
        byte buffer[size];

        /* if (read(parent->client_socket_, buffer, MAX_BUFFER_SIZE) < 0) {

             parent->running = false;
             running = false;
             return;
         }*/
        memcpy(buffer,&size,sizeof(unsigned short));
        helper_class::ReadXBytes(parent->client_socket_, size-2, buffer+2);
        parent->message_poller_queue.push(helper_class::buffer_to_array(buffer,size));

    }
    stop();


}

void message_poller::stop() {
    running = false;
    helper_class::display_info_syncronized_ln("Poller stop with id " + std::to_string(parent->index_));
    thread_b.join();
}


