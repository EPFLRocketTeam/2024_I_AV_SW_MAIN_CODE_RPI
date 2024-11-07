#include "task.h"

class CPUTask : public Task
{
public:
    CPUTask() : Task("Example task", 1e9, 80, 0) {}

protected:
    void loop() override;
};