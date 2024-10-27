#include "task.h"
#include <iostream>
#include <unistd.h>
using namespace std;

void *low_prio_function()
{
    cout << "Hello from low priority task!" << endl;
    return nullptr;
}

void *high_prio_function()
{
    cout << "Hello from high priority task!" << endl;
    return nullptr;
}

int main()
{
    // Testing that a low priority task can be preempted by a high priority task
    Task low_priority_task = Task("low priority task", 0, 80, 0, low_prio_function);
    Task high_priority_task = Task("high priority task", 0, 90, 0, high_prio_function);

    low_priority_task.start_task();
    sleep(10);
    high_priority_task.start_task();

    low_priority_task.wait_for_task();
    high_priority_task.wait_for_task();

    return 0;
}