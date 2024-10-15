#include "task.h"
#include <iostream>
using namespace std;

void *task_function_1()
{
    cout << "Hello world from task 1!" << endl;

    struct timespec time;
    time.tv_sec = 0;
    time.tv_nsec = 50e6;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &time, nullptr);

    return nullptr;
}

void *task_function_2()
{
    cout << "Hello world from task 1!" << endl;

    struct timespec time;
    time.tv_sec = 0;
    time.tv_nsec = 5e6;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &time, nullptr);

    return nullptr;
}

int main()
{
    Task tasks[] = {
        Task("task1", 100e6, 90, 0, task_function_1),
        Task("task2", 10e6, 80, 3, task_function_2)
    };

    for (auto &task : tasks) {
        task.start_task();
    }

    for (auto &task : tasks) {
        task.wait_for_task();
    }
    
    return 0;
}