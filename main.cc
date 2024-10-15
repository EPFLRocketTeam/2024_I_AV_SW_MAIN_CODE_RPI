#include "task.h"
#include <iostream>
using namespace std;

// Task function for task 1.
void *task_function_1()
{
    cout << "Hello from task 1!" << endl;
    return nullptr;
}
// Task function for task 2.
void *task_function_2()
{
    cout << "Hello from task 2!" << endl;
    return nullptr;
}
void *task_function_3()
{
    cout << "Hello from task 3!" << endl;
    return nullptr;
}
void *task_function_4()
{
    cout << "Hello from task 4!" << endl;
    return nullptr;
}

int main()
{
    // Initialize tasks with name, period, priority, CPU core, and function.
    Task tasks[] = {
        Task("task1", 100e6, 80, 0, task_function_1),
        Task("task2", 50e6, 80, 1, task_function_2),
        Task("task3", 150e6, 80, 2, task_function_3),
        Task("task4", 200e6, 80, 3, task_function_4)
    };

    // Start all tasks.
    for (auto &task : tasks) {
        task.start_task();
    }

    // Wait for all tasks to complete.
    for (auto &task : tasks) {
        task.wait_for_task();
    }

    return 0;
}