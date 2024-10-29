#ifndef LOGGING_H
#define LOGGING_H

#include <string>

enum LoggingLevel
{
    // The standard log level indicating that something happened.
    INFO,

    // The DEBUG log level should be used for information that may be needed for diagnosing issues and troubleshooting.
    DEBUG,
};

enum ErrorLevel
{
    // These errors do not compromise the flight.
    // They are not reported to GS.
    WARNING,

    // These error could compromise the flight, but the mission can continue.
    // They should be reported to GS.
    CRITICAL,

    // These errors should abort the mission automaticaly.
    // They should still be reported to GS.
    FATAL,
};

void print(LoggingLevel level, std::string message);
void error(ErrorLevel level, std::string message);

#endif