//
// Created by alaeddine on 12/27/17.
//

#ifndef FLOWOVER_GLOBAL_PROCESSIONG_VARS_H
#define FLOWOVER_GLOBAL_PROCESSIONG_VARS_H

#include "includes.h"
#include "constants_and_definitions.h"
#include "queues_over/sync_queue.h"

class global_processiong_vars {
public:
    static sync_queue<vector<byte >> multiplication_message_queue;

};

#endif //FLOWOVER_GLOBAL_PROCESSIONG_VARS_H
