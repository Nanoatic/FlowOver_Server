//
// Created by nanoatic on 08/10/17.
//

#ifndef FLOWOVER_INTERRUPT_EXCEPTION_H
#define FLOWOVER_INTERRUPT_EXCEPTION_H


#include "includes.h"

class interrupt_exception : public std::exception {
public:
    explicit interrupt_exception(int s) : S(s) {}

    int S;

    static void sig_to_exception(int s) {
        throw interrupt_exception(s);
    }
};


#endif //FLOWOVER_INTERRUPT_EXCEPTION_H
