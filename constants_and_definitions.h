//
// Created by nanoatic on 24/09/17.
//

#ifndef FLOWOVER_GLOBAL_H
#define FLOWOVER_GLOBAL_H

#define FILE_QUEUE
#undef FILE_QUEUE
#define  HANDICAP
//#undef HANDICAP
#define HANDICAP_VALUE 500

#define MAX_COLOR 255.0
#define SERVER_PORT 50000
#define MAX_BUFFER_SIZE 30000
#define MAX_QUEUE_SIZE 6000
#define APP_ID 0x30
#define DB_NAME "over_flow.db"

// locations

//mandatory
#define BUFFER_LENGTH_LOCATION 0
#define ID_LOCATION 2
#define INS_LOCATION 3
#define TAG_LOCATION 4

//optional
#define ROW_INDEX_LOCATION 5
#define COLUMN_INDEX_LOCATION 7
#define MATRIX_MULTIPLICATION_LENGTH_LOCATION 9
#define MATRIX_MULTIPLICATION_BEGIN_OF_DATA_LOCATION 11

// List of instructions
#define INS_MULTIPLY_AND_SUM_R_X_C 0x02

#define  KEEP_ALIVE 3

#define LENGTH_PADDING 12

#define LENGTH_PADDING_MANDATORY 5

#define MAX_CLIENTS 2
#define WAI_FOR_CLIENTS 1
typedef unsigned char byte;
#endif //FLOWOVER_GLOBAL_H
