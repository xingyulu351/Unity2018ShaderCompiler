#include "long_term_operation/LongTermOperController.h"
#include "long_term_operation/LongTermOperObserver.h"
#include "long_term_operation/LongTermOperProgressListener.h"

#include <list>
#include <map>
#include <algorithm>
#include <stdexcept>

namespace ureport
{
namespace details
{
    class LongTermOperController
        : public ureport::LongTermOperController
        , public ureport::LongTermOperObserver
    {
    public:
        LongTermOperController() : m_IsCanceled(false)
        {}

    private:
        LongTermOperController(const LongTermOperController&);

        std::list<LongTermOperProgressListener*> m_Listeners;
        bool m_IsCanceled;
        int m_PendingOperations;

        void Cancel() override
        {
            m_IsCanceled = true;
        }

        void AddProgressListener(LongTermOperProgressListener* listener) override
        {
            if (nullptr == listener)
                throw std::invalid_argument("listener cannot be nullptr");
            m_Listeners.push_back(listener);
        }

        void RemoveProgressListener(LongTermOperProgressListener* listener) override
        {
            m_Listeners.remove_if(
                [listener] (const LongTermOperProgressListener* l)
                { return l == listener; });
        }

        LongTermOperObserver* GetObserver() override
        {
            return this;
        }

        bool IsCanceled() const override
        {
            return m_IsCanceled;
        }

        void ReportProgress(size_t current, size_t total) override
        {
            std::for_each(
                m_Listeners.begin(),
                m_Listeners.end(),
                [this, current, total] (LongTermOperProgressListener* l)
                {
                    l->UpdateProgress(current, total);
                });
        }

        void ReportStatus(const std::string& status) override
        {
            std::for_each(
                m_Listeners.begin(),
                m_Listeners.end(),
                [this, &status] (LongTermOperProgressListener* l)
                {
                    l->UpdateStatus(status);
                });
        }

        bool IsOperationCanceled() const override
        {
            return m_IsCanceled;
        }
    };
}

    std::unique_ptr<LongTermOperController> CreateLongTermOperController()
    {
        return std::unique_ptr<LongTermOperController>(new details::LongTermOperController());
    }
}
