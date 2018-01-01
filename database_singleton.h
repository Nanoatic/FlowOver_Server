//
// Created by alaeddine on 11/18/17.
//

#ifndef FLOWOVER_DATABASE_SINGLETON_H
#define FLOWOVER_DATABASE_SINGLETON_H

#include "includes.h"
#include "constants_and_definitions.h"

class database_singleton {
private:
    static sqlite3 *db;
public:
    static void init();

    static sqlite3 *getInstance();

    static void close();
};


#endif //FLOWOVER_DATABASE_SINGLETON_H
