//
// Created by nanoatic on 07/10/17.
//

#include "keep_alive.h"


keep_alive::keep_alive(computing_client *f) : parent(f) {

}

void keep_alive::init() {

    helper_class::display_info_syncronized_ln("keep alive started with id " + std::to_string(parent->index_));

}

void keep_alive::run() {
    init();
    while (running) {
        sleep(KEEP_ALIVE);

        vector<byte> array(LENGTH_PADDING_MANDATORY);
        parent->message_dispatcher_queue.push(array);
    }
    stop();
}

void keep_alive::stop() {

    helper_class::display_info_syncronized_ln("keep alive stop with id " + std::to_string(parent->index_));
    running = false;
    thread_b.join();

}



