#include "log_interface.h"
#include <cactus_rt/rt.h>
#include <iostream>


LogInterface::LogInterface(const std::string& log_file_path, SharedMemory<ControlOutput>* shared_memory)
    : log_file_path(log_file_path), log_file(log_file_path, std::ios::out | std::ios::app), shared_memory(shared_memory) {
    if (!log_file.is_open()) {
        throw std::runtime_error("Failed to open log file: " + log_file_path);
    }
}

LogInterface::~LogInterface() {
    if (log_file.is_open()) log_file.close();
}

void LogInterface::LogData() {
    // Read from shared memory
    ControlOutput control_output = shared_memory->Read();

    // Log the data to the file
    log_file << "ControlOutput: " << control_output.d1 
    << ", "<< control_output.d2 << ", "<< control_output.thrust << ", "
    << control_output.mz << ", "<< std::endl; // Replace with actual fields
    log_file.flush();
}