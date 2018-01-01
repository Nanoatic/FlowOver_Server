//
// Created by alaeddine on 11/17/17.
//

#ifndef FLOWOVER_FILE_QUEUE_H
#define FLOWOVER_FILE_QUEUE_H


#include "../includes.h"
#include "../constants_and_definitions.h"

class file_queue {
public:
    file_queue(string name);

    ~file_queue();

    void push(vector<byte> vector_);

    vector<byte> first();

    void pop();

    bool is_empty();

private:
    static int callback_first(void *data, int argc, char **argv, char **azColName);

    static int callback_is_empty(void *data, int argc, char **argv, char **azColName);

    sqlite3 *db;
    string name;
    byte *data_first = nullptr;
    bool *data_is_empty = nullptr;
};


#endif //FLOWOVER_FILE_QUEUE_H
