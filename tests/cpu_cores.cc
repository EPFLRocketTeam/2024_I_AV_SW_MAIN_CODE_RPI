#include "task.h"
#include "configure.h"
#include <iostream>

class CPUTask : public Task
{
public:
    CPUTask(int n) : Task(std::to_string(n), 1e9, 80, n), n(n) {}

protected:
    int n;
    void loop() override
    {
        std::cout << "Hello from " << n << std::endl;
    };
};

int main()
{
    if (
        is_cpu_isolated(0) or
        is_cpu_isolated(1) or
        is_cpu_isolated(2) or
        is_cpu_isolated(3))
    {
        throw std::runtime_error("Some CPU cores are isolated. Please remove the isolation and reboot.");
    }

    CPUTask task0(0);
    CPUTask task1(1);
    CPUTask task2(2);
    CPUTask task3(3);

    task0.start_task();
    task1.start_task();
    task2.start_task();
    task3.start_task();

    task0.wait_for_task();
    task1.wait_for_task();
    task2.wait_for_task();
    task3.wait_for_task();
}