#include "progression/Progression.h"
#include "progression/Monitor.h"

#include <algorithm>

namespace unity
{
namespace progression
{
    class TaskProgress
    {
        size_t m_Steps;
        size_t m_Step;

    public:
        TaskProgress(size_t steps)
            : m_Steps(steps)
            , m_Step(0)
        {
        }

        void Advance()
        {
            ++m_Step;
        }

        float CalculateProgress(float subProgress) const
        {
            return (m_Step + subProgress) / m_Steps;
        }
    };

    void Progression::BeginFixedTask(size_t steps)
    {
        m_Tasks.push_back(std::make_shared<TaskProgress>(steps));
    }

    void Progression::Advance()
    {
        if (m_Tasks.empty())
            return;
        AdvanceCurrentTask();
        NotifyProgressChanged();
    }

    void Progression::EndTask()
    {
        if (m_Tasks.empty())
            return;
        NotifyProgressChanged();
        RemoveCurrentTask();
    }

    float Progression::GetTotal() const
    {
        return CalculateTotal();
    }

    void Progression::SetMonitor(std::shared_ptr<Monitor> monitor)
    {
        m_Monitor = monitor;
    }

    float Progression::CalculateTotal() const
    {
        float progress = 0;
        std::for_each(m_Tasks.rbegin(), m_Tasks.rend(),
            [&progress] (const std::shared_ptr<TaskProgress>& task) {
                const float taskProgress = task->CalculateProgress(progress);
                progress = taskProgress > 1 ? 1 : taskProgress;
            });
        return progress;
    }

    void Progression::NotifyProgressChanged() const
    {
        if (m_Monitor != nullptr)
            m_Monitor->ProgressChanged();
    }

    void Progression::AdvanceCurrentTask()
    {
        const auto& task = m_Tasks.back();
        task->Advance();
    }

    void Progression::RemoveCurrentTask()
    {
        m_Tasks.pop_back();
    }
}
}
