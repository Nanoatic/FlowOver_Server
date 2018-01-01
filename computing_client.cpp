//
// Created by nanoatic on 25/09/17.
//



#include "computing_client.h"
#include "global_processiong_vars.h"

computing_client::computing_client(int index, int client_socket, sockaddr_in client_address)
#ifdef FILE_QUEUE
:
message_poller_queue(string("queueP"+to_string(index))) ,
message_dispatcher_queue(string("queueD"+to_string(index)))
#endif
{
    md = new message_dispatcher(this);
    mp = new message_poller(this);
    ka = new keep_alive(this);
    client_socket_ = client_socket;
    client_address_ = client_address;
    index_ = index;


}

void computing_client::init() {

    in_addr ipAddr = client_address_.sin_addr;


    inet_ntop(AF_INET, &ipAddr, ip, INET_ADDRSTRLEN);


    md->start_md();
    mp->start_mp();
  //    ka->start_ka();

    helper_class::display_info_syncronized_ln(
            "Client connected @ " + std::string(ip) + " with id " + std::to_string(index_));
    server_things::connected_clients++;
}

void computing_client::run() {
    init();
    while (running) {
        vector<byte> buf;
#ifdef FILE_QUEUE
        if(!message_poller_queue.is_empty()) {
            buf = message_poller_queue.first();
            message_poller_queue.pop();
#else
        message_poller_queue.pop(buf);

#endif
        if (buf[ID_LOCATION] == APP_ID) {

            switch (buf[INS_LOCATION]) {

                case INS_MULTIPLY_AND_SUM_R_X_C: {
                    //forward to multiplcation queue
                    global_processiong_vars::multiplication_message_queue.push(buf);
                }
                    break;

                default:
                    break;
            }
        }


#ifdef FILE_QUEUE
        }
#endif

    }
    stop();
}


void computing_client::stop() {

    this->running = false;


    helper_class::display_info_syncronized_ln(
            "Client disconnected @ " + std::string(ip) + " with id " + std::to_string(index_));

    ka->running = false;
    md->running = false;
    mp->running = false;
    delete ka;
    delete md;
    delete mp;
    server_things::connected_clients--;
    server_things::available_slots[index_] = true;
    thread_b.join();
}





