#pragma once

#include "progression/Progression.h"
#include "long_term_operation/LongTermOperObserver.h"

#include <memory>
#include <stack>

namespace ureport
{
    class ProgressionOperationObserver : public LongTermOperObserver
    {
        struct Task
        {
            size_t m_Total;
            size_t m_Progress;

            Task(size_t total)
                : m_Total(total)
                , m_Progress(0)
            {
            }
        };

        unity::progression::Progression& m_Progression;
        LongTermOperObserver& m_Original;
        std::stack<Task> m_Tasks;

    public:
        ProgressionOperationObserver(unity::progression::Progression& progression,
                                     LongTermOperObserver& original)
            : m_Progression(progression)
            , m_Original(original)
        {
        }

    private:
        void ReportProgress(size_t current, size_t total) override
        {
            if (IsOperationCanceled())
                return;
            if (current == 0)
                BeginTask(total);
            if (current > 0)
                AdvanceTask(current);
            ReportProgress();
            if (current == m_Tasks.top().m_Total)
                EndTask();
        }

        void BeginTask(size_t total)
        {
            m_Progression.BeginFixedTask(total);
            m_Tasks.push(Task(total));
        }

        void AdvanceTask(size_t progress)
        {
            const size_t delta = progress - m_Tasks.top().m_Progress;
            for (size_t i = 0; i < delta; ++i)
                m_Progression.Advance();
            m_Tasks.top().m_Progress = progress;
        }

        void EndTask()
        {
            m_Progression.EndTask();
            m_Tasks.pop();
        }

        void ReportProgress()
        {
            m_Original.ReportProgress(m_Progression.GetTotal() * 100, 100);
        }

        void ReportStatus(const std::string& status) override
        {
            m_Original.ReportStatus(status);
        }

        bool IsOperationCanceled() const override
        {
            return m_Original.IsOperationCanceled();
        }
    };
}
