#include "task.h"
#include <iostream>
#include <vector>

class CPUTask : public Task
{
public:
    CPUTask(int n) : Task("Task " + n, 1e9, 80, n), n(n) {}

protected:
    int n;
    void loop() override {
        std::cout << "Hello from " << n << std::endl;
    };
};

int main()
{
    std::vector<CPUTask> tasks;
    for (int i = 0; i < 4; i++)
    {
        CPUTask task(i);
        task.start_task();
        tasks.push_back(task);
    }
    for (auto &task : tasks)
    {
        task.wait_for_task();
    }
}