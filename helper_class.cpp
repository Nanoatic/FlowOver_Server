//
// Created by alaeddine on 10/21/17.
//

#include "helper_class.h"

void helper_class::display_info_syncronized_ln(std::string message) {
    stringstream stream;
    stream << message << "\n";
    cout << stream.str();
}

void helper_class::display_error_syncronized_ln(std::string message) {
    stringstream stream;
    stream << message << "\n";
    cerr << stream.str();
}

void helper_class::display_info_syncronized(std::string message) {
    stringstream stream;
    stream << message;
    cout << stream.str();
}

void helper_class::display_error_syncronized(std::string message) {
    stringstream stream;
    stream << message;
    cerr << stream.str();
}

vector<byte> helper_class::buffer_to_array(const byte *buffer ,size_t size) {
    vector<byte> array_(size);
    for (int i = 0; i < size; i++) {
        array_[i] = buffer[i];
    }
    return array_;

}






