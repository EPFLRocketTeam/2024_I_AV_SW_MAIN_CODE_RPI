#include "logging.h"
#include <iostream>
using namespace std;

void print(LoggingLevel level, std::string message) {
    cout << message << endl;
}

void error(ErrorLevel level, std::string message) {
    cerr << "Error: " << message << endl;

    // TODO: need to use the state machine in flight.
    if (level == FATAL) {
        exit(EXIT_FAILURE);
    }
}

// Possible improvements:
// - Indicate level of logging
// - Add timestamp to log messages
// - Add logging to file
// https://www.geeksforgeeks.org/logging-system-in-cpp/