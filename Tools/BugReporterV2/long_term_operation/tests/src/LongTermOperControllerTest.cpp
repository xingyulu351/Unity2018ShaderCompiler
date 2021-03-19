#include "long_term_operation/LongTermOperController.h"
#include "long_term_operation/LongTermOperObserver.h"
#include "LongTermOperProgressListenerSpy.h"

#include <UnitTest++.h>

#include <stdexcept>

using namespace ureport::test;

SUITE(LongTermOperController)
{
    struct Fixture
    {
        std::unique_ptr<ureport::LongTermOperController> controller;
        ureport::LongTermOperObserver* observer;
        Fixture()
            : controller(ureport::CreateLongTermOperController())
            , observer(controller->GetObserver())
        {
        }
    };

    TEST_FIXTURE(Fixture, AddProgressListener_GivenNullPtrListener_ThrowsException)
    {
        CHECK_THROW(controller->AddProgressListener(nullptr), std::invalid_argument);
    }

    TEST_FIXTURE(Fixture, GetObserver_Returns_NotNullPtr)
    {
        CHECK(controller->GetObserver() != nullptr);
    }

    TEST_FIXTURE(Fixture, ReportProgress_GivenListener_CallsUpdateProgress)
    {
        LongTermOperProgressListenerSpy listener;
        controller->AddProgressListener(&listener);
        controller->GetObserver()->ReportProgress(2, 10);
        CHECK_EQUAL(2, listener.m_Current);
        CHECK_EQUAL(10, listener.m_Total);
    }

    TEST_FIXTURE(Fixture, ReportStatus_GivenListener_CallsUpdateStatus)
    {
        LongTermOperProgressListenerSpy listener;
        controller->AddProgressListener(&listener);
        controller->GetObserver()->ReportStatus("status");
        CHECK_EQUAL("status", listener.m_Status.c_str());
    }

    TEST_FIXTURE(Fixture, ReportProgress_DoesntCallsUpdateProgressForRemovedListener)
    {
        LongTermOperProgressListenerSpy listener;
        listener.m_Current = -1;
        listener.m_Total = -1;
        controller->AddProgressListener(&listener);
        controller->RemoveProgressListener(&listener);
        controller->GetObserver()->ReportProgress(2, 10);
        CHECK_EQUAL(-1, listener.m_Current);
        CHECK_EQUAL(-1, listener.m_Total);
    }

    TEST_FIXTURE(Fixture, ReportStatus_DoesntCallsUpdateStatusForRemovedListener)
    {
        LongTermOperProgressListenerSpy listener;
        listener.m_Status = "empty";
        controller->AddProgressListener(&listener);
        controller->RemoveProgressListener(&listener);
        controller->GetObserver()->ReportStatus("status");
        CHECK_EQUAL("empty", listener.m_Status.c_str());
    }

    TEST_FIXTURE(Fixture, IsOperationCanceled_InitiallyReturnsFalse)
    {
        CHECK(!observer->IsOperationCanceled());
    }

    TEST_FIXTURE(Fixture, IsOperationCanceled_GivenCancelOperationsCall_ReturnsTrue)
    {
        controller->Cancel();
        CHECK(observer->IsOperationCanceled());
    }

    TEST_FIXTURE(Fixture, IsCanceled_InitiallyReturnsFalse)
    {
        CHECK(!controller->IsCanceled());
    }

    TEST_FIXTURE(Fixture, IsCanceled_GivenCancelOperationsCall_ReturnsTrue)
    {
        controller->Cancel();
        CHECK(controller->IsCanceled());
    }
}
