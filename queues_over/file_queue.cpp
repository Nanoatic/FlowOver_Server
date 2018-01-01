//
// Created by alaeddine on 11/17/17.
//

#include "file_queue.h"
#include "../helper_class.h"
#include "../database_singleton.h"


file_queue::file_queue(string name) {
    int rc;
    string sql;
    char *zErrMsg = nullptr;
    this->name = name;
    db = database_singleton::getInstance();
    /* Create SQL statement */
    sql = "CREATE TABLE IF NOT EXISTS " + name + "("  \
         "ID INTEGER PRIMARY KEY AUTOINCREMENT   ," \
         "BUFFER          TEXT    NOT NULL" \
         " );";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, string(name + "SQL error: %s\n").c_str(), zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, string(name + " created successfully\n").c_str());
    }
}

file_queue::~file_queue() {
    string sql;
    char *zErrMsg = nullptr;
    int rc;

    sql = "DROP TABLE IF EXISTS " + name + ";";
    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, string(name + "SQL error: %s\n").c_str(), zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, string(name + " destroyed successfully\n").c_str());
    }
}

void file_queue::push(vector<byte> vector_) {
    string sql;
    char *zErrMsg = nullptr;
    int rc;

    sql = "INSERT INTO " + name + " VALUES( NULL, '" + string((char *) vector_.data()) + "');";
    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, string(name + "SQL error: %s\n").c_str(), zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, string(name + " pushed vector successfully\n").c_str());
    }
}

vector<byte> file_queue::first() {
    string sql;
    char *zErrMsg = nullptr;
    int rc;

    sql = "SELECT * FROM " + name + " LIMIT 1;";
    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql.c_str(), callback_first, this, &zErrMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, string(name + "SQL error: %s\n").c_str(), zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, string(name + " got first successfully\n").c_str());
    }
    vector<byte> return_vector = helper_class::buffer_to_array(data_first , MAX_BUFFER_SIZE);
    delete[] data_first;
    return return_vector;
}

void file_queue::pop() {
    string sql;
    char *zErrMsg = nullptr;
    int rc;
    sql = "Delete from " + name + " where ID IN (Select ID from " + name + " LIMIT 1);";
    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, string(name + "SQL error: %s\n").c_str(), zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, string(name + " popped vector successfully\n").c_str());
    }


}

int file_queue::callback_first(void *data, int argc, char **argv, char **azColName) {
    auto *object = (file_queue *) data;
    object->data_first = new byte[MAX_BUFFER_SIZE];
    if (argc > 0) {
        for (int i = 0; i < MAX_BUFFER_SIZE; ++i) {
            object->data_first[i] = argv[1][i];
        }
    }

    return 0;
}

bool file_queue::is_empty() {
    string sql;
    char *zErrMsg = 0;
    int rc;
    sql = " SELECT exists(SELECT 1 FROM " + name + " LIMIT 1);  ";
    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql.c_str(), callback_is_empty, this, &zErrMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, string(name + " SQL error: %s\n").c_str(), zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, string(name + " empty check success\n").c_str());
    }
    bool e = *data_is_empty;
    delete data_is_empty;
    return e;

}

int file_queue::callback_is_empty(void *data, int argc, char **argv, char **azColName) {
    file_queue *object = (file_queue *) data;
    object->data_is_empty = new bool;
    if (argc > 0) {

        int count;
        count = atoi(argv[0]);
        *object->data_is_empty = (count == 0);
    }

    return 0;
}
