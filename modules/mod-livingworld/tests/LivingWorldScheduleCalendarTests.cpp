#include "LivingWorldScheduleCalendar.h"

#include <cassert>
#include <iostream>

using namespace LivingWorld;

int main()
{
    WeeklySchedulePlan plan;
    plan.dayMask = static_cast<std::uint8_t>((1U << 1) | (1U << 4));
    plan.preferredStartMinute = 1080;
    plan.plannedSessionMinutes = 120;
    plan.skipSessionChance = 0;
    plan.marathonEligible = false;

    auto first = ScheduleCalendar::BuildUpcoming(12345, plan, 10, 3, 5, 21, 0);
    auto second = ScheduleCalendar::BuildUpcoming(12345, plan, 10, 3, 5, 21, 0);

    assert(first.size() == 5);
    assert(second.size() == first.size());

    for (std::size_t index = 0; index < first.size(); ++index)
    {
        assert(first[index].weekIndex == second[index].weekIndex);
        assert(first[index].dayIndex == second[index].dayIndex);
        assert(first[index].occurrence.startMinute == second[index].occurrence.startMinute);
        assert(first[index].occurrence.sessionMinutes == second[index].occurrence.sessionMinutes);
        assert(!first[index].occurrence.skipped);
    }

    assert(first[0].weekIndex == 10 && first[0].dayIndex == 4);
    assert(first[1].weekIndex == 11 && first[1].dayIndex == 1);
    assert(first[2].weekIndex == 11 && first[2].dayIndex == 4);

    auto bounded = ScheduleCalendar::BuildUpcoming(12345, plan, 0, 0, 1000, 1000, 0);
    assert(bounded.size() <= 64);

    WeeklySchedulePlan empty;
    assert(ScheduleCalendar::BuildUpcoming(1, empty, 0, 0, 10).empty());
    assert(ScheduleCalendar::BuildUpcoming(1, plan, 0, 0, 0).empty());
    assert(ScheduleCalendar::BuildUpcoming(1, plan, 0, 0, 10, 0).empty());

    std::cout << "LivingWorld schedule calendar tests passed.\n";
    return 0;
}
