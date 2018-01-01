//
// Created by nanoatic on 07/10/17.
//

#include "message_dispatcher.h"


message_dispatcher::message_dispatcher(computing_client *f) : parent(f) {

}

void message_dispatcher::init() {
    helper_class::display_info_syncronized_ln("Dispatcher started with id " + std::to_string(parent->index_));
}

void message_dispatcher::run() {

    init();

    while (running) {


        vector<byte> buf;
#ifdef FILE_QUEUE
        if(!parent->message_dispatcher_queue.is_empty()) {
            buf = parent->message_dispatcher_queue.first();
            parent->message_dispatcher_queue.pop();
#else
        parent->message_dispatcher_queue.pop(buf);
#endif

        try {


            try {
                /*  struct sigaction sigIntHandler{};
                  sigIntHandler.sa_handler = interrupt_exception::sig_to_exception;
                  sigemptyset(&sigIntHandler.sa_mask);
                  sigIntHandler.sa_flags = 0;
                  sigaction(SIGPIPE, &sigIntHandler, nullptr);*/
                // write(parent->client_socket_, buffer, MAX_BUFFER_SIZE);
                helper_class::send_all(parent->client_socket_, buf.data(), buf.size());

            } catch (interrupt_exception &e) {
                running = false;
                parent->running = false;
            }

        } catch (const std::exception &e) {

            parent->running = false;
        }

#ifdef FILE_QUEUE
        }
#endif
    }
    stop();


}


void message_dispatcher::stop() {
    running = false;
    helper_class::display_info_syncronized_ln("Dispatcher stop with id " + std::to_string(parent->index_));
    thread_b.join();

}


