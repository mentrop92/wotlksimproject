#include "LivingWorldScheduleOccurrence.h"

#include <cassert>
#include <iostream>

using namespace LivingWorld;

int main()
{
    WeeklySchedulePlan plan;
    plan.dayMask = 0b0010101;
    plan.preferredStartMinute = 1200;
    plan.plannedSessionMinutes = 180;
    plan.skipSessionChance = 0;
    plan.marathonEligible = false;

    ScheduleOccurrence const first = ScheduleOccurrencePlanner::Build(123456ULL, plan, 8, 2, 30);
    ScheduleOccurrence const second = ScheduleOccurrencePlanner::Build(123456ULL, plan, 8, 2, 30);
    assert(!first.skipped);
    assert(!first.marathon);
    assert(first.startMinute == second.startMinute);
    assert(first.sessionMinutes == second.sessionMinutes);
    assert(first.startMinute < 1440);
    assert(first.sessionMinutes >= 160);
    assert(first.sessionMinutes <= 200);

    ScheduleOccurrence const unscheduled = ScheduleOccurrencePlanner::Build(123456ULL, plan, 8, 1, 30);
    assert(unscheduled.skipped);
    assert(unscheduled.sessionMinutes == 0);

    ScheduleOccurrence const invalidDay = ScheduleOccurrencePlanner::Build(123456ULL, plan, 8, 7, 30);
    assert(invalidDay.skipped);

    plan.skipSessionChance = 100;
    ScheduleOccurrence const forcedSkip = ScheduleOccurrencePlanner::Build(987654ULL, plan, 2, 0, 30);
    assert(forcedSkip.skipped);

    plan.skipSessionChance = 0;
    plan.marathonEligible = true;
    bool foundMarathon = false;
    for (std::uint32_t week = 0; week < 100; ++week)
    {
        ScheduleOccurrence const occurrence = ScheduleOccurrencePlanner::Build(777ULL, plan, week, 0, 500);
        assert(occurrence.startMinute < 1440);
        assert(occurrence.sessionMinutes >= 15);
        assert(occurrence.sessionMinutes <= 720);
        if (occurrence.marathon)
        {
            foundMarathon = true;
            assert(occurrence.sessionMinutes >= 340);
        }
    }
    assert(foundMarathon);

    std::cout << "LivingWorld schedule occurrence tests passed.\n";
    return 0;
}
