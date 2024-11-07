#include "task.h"

class ExampleTask : public Task
{
public:
    ExampleTask() : Task("Example task", 1e9, 80, 0) {}

protected:
    void loop() override;
};