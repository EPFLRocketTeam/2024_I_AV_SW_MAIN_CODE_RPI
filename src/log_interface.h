#ifndef LOG_INTERFACE_H
#define LOG_INTERFACE_H

#include <string>
#include <fstream>
#include "shared_memory.h"
#include "DroneController.h"
#include <cactus_rt/rt.h>

class LogInterface {
public:
    LogInterface(const std::string& log_file_path, SharedMemory<ControlOutput>* shared_memory);
    ~LogInterface();

    void LogData();

private:
    std::string log_file_path;
    std::ofstream log_file;
    SharedMemory<ControlOutput>* shared_memory;
};

#endif // LOG_INTERFACE_H