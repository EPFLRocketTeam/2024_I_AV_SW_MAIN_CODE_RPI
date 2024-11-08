// Tests that high priority tasks preempt lower priority tasks.
// Expected behavior: (check with ps)
// 1. The task with priority 80 should saturate the terminal with "Priority: 80" messages.
// 2. After 10 seconds, the task with priority 90 should saturate the terminal with "Priority: 90" messages.

#include "task.h"
#include "configure.h"
#include <iostream>
#include <unistd.h>

class PriorityTask : public Task
{
public:
    PriorityTask(int priority) : Task(std::to_string(priority), 0, priority, 0), priority(priority) {}

protected:
    int priority;
    void loop() override
    {
        std::cout << "Priority: " << priority << std::endl;
    };
};

int main()
{
    if (is_cpu_isolated(0))
    {
        throw std::runtime_error("CPU core 0 is isolated. Please remove the isolation and reboot.");
    }

    PriorityTask task0(80);
    PriorityTask task1(90);

    task0.start_task();
    sleep(10);
    task1.start_task();

    task0.wait_for_task();
    task1.wait_for_task();
}