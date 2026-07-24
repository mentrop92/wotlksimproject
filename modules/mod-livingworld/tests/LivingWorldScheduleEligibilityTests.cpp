#include "LivingWorldScheduleEligibility.h"

#include <cassert>
#include <iostream>

using namespace LivingWorld;

int main()
{
    WeeklySchedulePlan plan;
    plan.dayMask = 0b0000100; // day index 2
    plan.preferredStartMinute = 18U * 60U;

    ScheduleEligibilityInput input;
    input.dayOfWeek = 2;
    input.minuteOfDay = 17U * 60U + 50U;

    ScheduleEligibilityResult result = ScheduleEligibility::Evaluate(plan, input);
    assert(result.scheduledToday);
    assert(result.withinLoginWindow);
    assert(result.eligible);
    assert(result.minutesUntilWindow == 0);

    input.alreadyOnline = true;
    result = ScheduleEligibility::Evaluate(plan, input);
    assert(!result.eligible);

    input.alreadyOnline = false;
    input.loginCooldownActive = true;
    result = ScheduleEligibility::Evaluate(plan, input);
    assert(!result.eligible);

    input.loginCooldownActive = false;
    input.minuteOfDay = 16U * 60U;
    result = ScheduleEligibility::Evaluate(plan, input);
    assert(!result.withinLoginWindow);
    assert(result.minutesUntilWindow == 105);

    // From the day after the only scheduled day, the next early window is five
    // days and 23 hours 45 minutes away (Wednesday 17:45 from Thursday 18:00).
    input.dayOfWeek = 3;
    input.minuteOfDay = 18U * 60U;
    result = ScheduleEligibility::Evaluate(plan, input);
    assert(!result.scheduledToday);
    assert(!result.eligible);
    assert(result.minutesUntilWindow == 8625);

    // After today's window closes, the evaluator rolls to the same weekday next week.
    input.dayOfWeek = 2;
    input.minuteOfDay = 20U * 60U;
    result = ScheduleEligibility::Evaluate(plan, input);
    assert(result.scheduledToday);
    assert(!result.withinLoginWindow);
    assert(result.minutesUntilWindow == 9945);

    // Invalid empty schedules remain bounded and never become eligible.
    plan.dayMask = 0;
    result = ScheduleEligibility::Evaluate(plan, input);
    assert(!result.scheduledToday);
    assert(!result.eligible);
    assert(result.minutesUntilWindow == 10080);

    std::cout << "LivingWorld schedule eligibility tests passed.\n";
    return 0;
}
