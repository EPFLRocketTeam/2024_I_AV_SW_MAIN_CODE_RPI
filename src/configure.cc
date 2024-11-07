#include "configure.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

// Could potentially be made simpler by using sched_getaffinity :shrug:
bool is_cpu_isolated(int cpu_number) {
    std::ifstream file("/boot/firmware/cmdline.txt");
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open /boot/firmware/cmdline.txt\nAre you running on a Raspberry Pi?");
    }


    std::string line;
    std::getline(file, line);
    file.close();

    std::string isolcpus_str = "isolcpus=";
    size_t isolcpus_pos = line.find(isolcpus_str);
    if (isolcpus_pos == std::string::npos) {
        return false;
    }

    // Extract the substring starting from 'isolcpus='
    size_t start_pos = isolcpus_pos + isolcpus_str.length();
    size_t end_pos = line.find(' ', start_pos);
    std::string cpu_list_str = line.substr(start_pos, end_pos - start_pos);

    // Split the CPU numbers by comma
    std::vector<std::string> cpu_list;
    std::stringstream ss(cpu_list_str);
    std::string cpu;
    while (std::getline(ss, cpu, ',')) {
        cpu_list.push_back(cpu);
    }

    // Check if the cpu_number is in the list
    if (std::find(cpu_list.begin(), cpu_list.end(), std::to_string(cpu_number)) == cpu_list.end()) {
        return false;
    }

    return true;
}

// Things to add in this file:
// - Memory locking
// - Tune turbo and system cpu limits: https://forums.raspberrypi.com/viewtopic.php?t=228727
