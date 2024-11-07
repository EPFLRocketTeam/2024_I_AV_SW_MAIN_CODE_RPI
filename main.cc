#include "example_task.h"

int main()
{
    ExampleTask example_task;
    example_task.start_task();
    example_task.wait_for_task();
    return 0;
}