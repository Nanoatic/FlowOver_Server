//
// Created by alaeddine on 11/18/17.
//

#include "database_singleton.h"

sqlite3 *database_singleton::db = nullptr;

sqlite3 *database_singleton::getInstance() {
    return db;
}

void database_singleton::close() {
    sqlite3_close(db);

}

void database_singleton::init() {
    int rc = sqlite3_open(DB_NAME, &db);

    if (rc) {
        fprintf(stderr, " Can't open database: %s\n", sqlite3_errmsg(db));
        return;
    } else {
        fprintf(stdout, "Opened database successfully\n");
    }

}
