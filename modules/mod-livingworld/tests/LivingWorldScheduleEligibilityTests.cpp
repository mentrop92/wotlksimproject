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

    input.dayOfWeek = 3;
    input.minuteOfDay = 18U * 60U;
    result = ScheduleEligibility::Evaluate(plan, input);
    assert(!result.scheduledToday);
    assert(!result.eligible);
    assert(result.minutesUntilWindow == 360);

    std::cout << "LivingWorld schedule eligibility tests passed.\n";
    return 0;
}
