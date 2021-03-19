#pragma once

#include <vector>
#include <memory>

namespace unity
{
namespace progression
{
    class TaskProgress;
    class Monitor;

    class Progression
    {
        std::vector<std::shared_ptr<TaskProgress> > m_Tasks;
        std::shared_ptr<Monitor> m_Monitor;

    public:
        void BeginFixedTask(size_t steps);

        void Advance();

        void EndTask();

        float GetTotal() const;

        void SetMonitor(std::shared_ptr<Monitor> monitor);

    private:
        float CalculateTotal() const;

        void NotifyProgressChanged() const;

        void AdvanceCurrentTask();

        void RemoveCurrentTask();
    };
}
}
