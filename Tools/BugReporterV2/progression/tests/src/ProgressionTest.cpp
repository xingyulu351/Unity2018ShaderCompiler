#include "progression/Progression.h"
#include "MonitorSpy.h"

#include <UnitTest++.h>

SUITE(FixedTask)
{
    using namespace unity::progression;
    using namespace unity::progression::test;

    class AProgression
    {
    public:
        Progression m_Progression;
    };

    class FixedTaskWithTwoSteps : public AProgression
    {
    public:
        FixedTaskWithTwoSteps()
        {
            m_Progression.BeginFixedTask(2);
        }
    };

    class MonitoredProgression : public AProgression
    {
    public:
        std::shared_ptr<MonitorSpy> m_Monitor;

        MonitoredProgression()
            : m_Monitor(std::make_shared<MonitorSpy>())
        {
            m_Progression.SetMonitor(m_Monitor);
        }
    };

    TEST_FIXTURE(FixedTaskWithTwoSteps, GetTotal_WhenNoStepsAndvanced_ReturnsZero)
    {
        CHECK_EQUAL(0, m_Progression.GetTotal());
    }

    TEST_FIXTURE(FixedTaskWithTwoSteps, GetTotal_AfterOneStep_ReturnsHalf)
    {
        m_Progression.Advance();
        CHECK_EQUAL(0.5, m_Progression.GetTotal());
    }

    TEST_FIXTURE(FixedTaskWithTwoSteps, GetTotal_EndTaskWhenNoStepAdvanced_ReturnsZero)
    {
        m_Progression.EndTask();
        CHECK_EQUAL(0, m_Progression.GetTotal());
    }

    TEST_FIXTURE(FixedTaskWithTwoSteps, GetTotal_AfterAdvancedTwoStep_ReturnsOne)
    {
        m_Progression.Advance();
        m_Progression.Advance();
        CHECK_EQUAL(1, m_Progression.GetTotal());
    }

    TEST_FIXTURE(FixedTaskWithTwoSteps, GetTotal_BeginNewFixedTaskOnFirstStep_ReturnsHalf)
    {
        m_Progression.BeginFixedTask(1);
        CHECK_EQUAL(0, m_Progression.GetTotal());
    }

    TEST_FIXTURE(FixedTaskWithTwoSteps, GetTotal_BeginNewFixedTaskOnFirstStepThenAdvance_ReturnsHalf)
    {
        m_Progression.BeginFixedTask(1);
        m_Progression.Advance();
        CHECK_EQUAL(0.5, m_Progression.GetTotal());
    }

    TEST_FIXTURE(FixedTaskWithTwoSteps, GetTotal_BeginNewFixedTaskOnSecondStep_ReturnsHalf)
    {
        m_Progression.Advance();
        m_Progression.BeginFixedTask(1);
        CHECK_EQUAL(0.5, m_Progression.GetTotal());
    }

    TEST_FIXTURE(FixedTaskWithTwoSteps, GetTotal_BeginNewTaskOnSecondStepThenAdvance_ReturnsOne)
    {
        m_Progression.Advance();
        m_Progression.BeginFixedTask(1);
        m_Progression.Advance();
        CHECK_EQUAL(1, m_Progression.GetTotal());
    }

    TEST_FIXTURE(AProgression, GetTotal_AfterAdvancedDeepestOfThreeNestedFixedTasksWithTwoSteps_ReturnsEighth)
    {
        m_Progression.BeginFixedTask(2);
        m_Progression.BeginFixedTask(2);
        m_Progression.BeginFixedTask(2);
        m_Progression.Advance();
        CHECK_EQUAL(0.125, m_Progression.GetTotal());
    }

    TEST_FIXTURE(AProgression, GetTotal_ThreeNestedFixedTasksWithTwoStepsAdvancedAfterStart_ReturnsLastEighthPart)
    {
        m_Progression.BeginFixedTask(2);
        m_Progression.Advance();
        m_Progression.BeginFixedTask(2);
        m_Progression.Advance();
        m_Progression.BeginFixedTask(2);
        m_Progression.Advance();
        CHECK_EQUAL(0.875, m_Progression.GetTotal());
    }

    TEST_FIXTURE(AProgression, GetTotal_AfterTwoSequentialFixedTasksFullyAdvanced_ReturnsOne)
    {
        m_Progression.BeginFixedTask(1);
        m_Progression.Advance();
        m_Progression.EndTask();
        m_Progression.BeginFixedTask(1);
        m_Progression.Advance();
        CHECK_EQUAL(1, m_Progression.GetTotal());
    }

    TEST_FIXTURE(AProgression, GetTotal_AdvanceFixedTaskWithOneStepByOneExtraStep_ReturnsOne)
    {
        m_Progression.BeginFixedTask(1);
        m_Progression.Advance();
        m_Progression.Advance();
        CHECK_EQUAL(1, m_Progression.GetTotal());
    }

    TEST_FIXTURE(AProgression, Advance_WhenNoTaskStarted_DoesNothing)
    {
        m_Progression.Advance();
        CHECK_EQUAL(0, m_Progression.GetTotal());
    }

    TEST_FIXTURE(AProgression, EndTask_WhenNoTaskStarted_DoesNothing)
    {
        m_Progression.EndTask();
        CHECK_EQUAL(0, m_Progression.GetTotal());
    }

    TEST_FIXTURE(MonitoredProgression, Advance_CallsProgressChangedOfTheMonitor)
    {
        m_Progression.BeginFixedTask(1);
        m_Progression.Advance();
        CHECK_EQUAL(1, m_Monitor->m_ProgressChangeCount);
    }

    TEST_FIXTURE(MonitoredProgression, EndTask_CallsProgressChangedOfTheMonitor)
    {
        m_Progression.BeginFixedTask(1);
        m_Progression.EndTask();
        CHECK_EQUAL(1, m_Monitor->m_ProgressChangeCount);
    }
}
