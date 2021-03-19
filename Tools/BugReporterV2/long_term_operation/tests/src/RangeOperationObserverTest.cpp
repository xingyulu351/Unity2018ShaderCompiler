#include "long_term_operation/RangeOperationObserver.h"
#include "OperationObserverSpy.h"

#include <UnitTest++.h>
#include <memory>

SUITE(RangeOperationObserver)
{
    using namespace ureport;
    using namespace ureport::test;

    class Observer
    {
    public:
        std::shared_ptr<OperationObserverSpy> m_Original;
        RangeOperationObserver m_Observer;

        Observer()
            : m_Original(std::make_shared<OperationObserverSpy>())
            , m_Observer(m_Original)
        {
        }
    };

    TEST_FIXTURE(Observer, ReportStatus_ReportsStatusToOriginalObserver)
    {
        m_Observer.ReportStatus("status");
        CHECK_EQUAL("status", m_Original->m_Status);
    }

    TEST_FIXTURE(Observer, ReportProgress_GivenZeroProgress_ReportsLowerBoundToOriginal)
    {
        m_Observer.SetRange(1, 2);
        m_Observer.ReportProgress(0, 1);
        CHECK_EQUAL(1, m_Original->m_Progress);
    }

    TEST_FIXTURE(Observer, ReportProgress_GivenProgressEqualTotal_ReportsUpperBoundToOriginal)
    {
        m_Observer.SetRange(1, 2);
        m_Observer.ReportProgress(2, 2);
        CHECK_EQUAL(2, m_Original->m_Progress);
    }

    TEST_FIXTURE(Observer, ReportProgress_GivenProgressHalfTotal_ReportsMiddleToOriginal)
    {
        m_Observer.SetRange(1, 3);
        m_Observer.ReportProgress(1, 2);
        CHECK_EQUAL(2, m_Original->m_Progress);
    }

    TEST_FIXTURE(Observer, ReportProgress_GivenZeroTotal_ReportsNothingToOriginal)
    {
        m_Observer.SetRange(1, 3);
        m_Original->m_Progress = -1;
        m_Observer.ReportProgress(0, 0);
        CHECK_EQUAL(-1, m_Original->m_Progress);
    }

    TEST_FIXTURE(Observer, IsOperationCanceled_WhenOriginalObserverAnswersFalse_ReturnsFalse)
    {
        m_Original->m_Canceled = false;
        CHECK_EQUAL(false, m_Observer.IsOperationCanceled());
    }

    TEST_FIXTURE(Observer, IsOperationCanceled_WhenOriginalObserverAnswersTrue_ReturnsTrue)
    {
        m_Original->m_Canceled = true;
        CHECK_EQUAL(true, m_Observer.IsOperationCanceled());
    }

    TEST_FIXTURE(Observer, ReportProgress_GivenZeroBounds_ReportsZeroToOriginal)
    {
        m_Observer.SetRange(0, 0);
        m_Observer.ReportProgress(1, 2);
        CHECK_EQUAL(0, m_Original->m_Progress);
    }

    TEST_FIXTURE(Observer, ReportProgress_GivenNonZeroEqualBounds_ReportsLowerToOriginal)
    {
        m_Observer.SetRange(1, 1);
        m_Observer.ReportProgress(1, 2);
        CHECK_EQUAL(1, m_Original->m_Progress);
        m_Observer.SetRange(3, 3);
        m_Observer.ReportProgress(1, 2);
        CHECK_EQUAL(3, m_Original->m_Progress);
    }
}
