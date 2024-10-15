#include "task.h"
#include <iostream>
using namespace std;

void *task_function_1()
{
    cout << "Hello from task 1!" << endl;
    return nullptr;
}

void *task_function_2()
{
    cout << "Hello from task 2!" << endl;
    return nullptr;
}

int main()
{
    Task tasks[] = {
        Task("task1", 100e6, 90, 0, task_function_1),
        Task("task2", 50e6, 80, 3, task_function_2)
    };

    for (auto &task : tasks) {
        task.start_task();
    }

    for (auto &task : tasks) {
        task.wait_for_task();
    }

    return 0;
}