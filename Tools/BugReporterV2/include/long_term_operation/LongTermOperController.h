#pragma once

#include <functional>
#include <memory>

namespace ureport
{
    class LongTermOperObserver;
    class LongTermOperProgressListener;

    class LongTermOperController
    {
    public:
        virtual void Cancel() = 0;
        virtual void AddProgressListener(LongTermOperProgressListener*) = 0;
        virtual void RemoveProgressListener(LongTermOperProgressListener*) = 0;
        virtual LongTermOperObserver* GetObserver() = 0;
        virtual bool IsCanceled() const = 0;
        virtual ~LongTermOperController() {}
    };

    std::unique_ptr<LongTermOperController> CreateLongTermOperController();
}
