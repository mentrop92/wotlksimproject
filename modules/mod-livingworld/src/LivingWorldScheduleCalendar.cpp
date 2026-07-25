#include "LivingWorldScheduleCalendar.h"

#include <algorithm>

namespace LivingWorld
{
    std::vector<ScheduledSession> ScheduleCalendar::BuildUpcoming(
        std::uint64_t profileSeed,
        WeeklySchedulePlan const& weeklyPlan,
        std::uint32_t startWeekIndex,
        std::uint8_t startDayIndex,
        std::uint32_t maximumSessions,
        std::uint32_t maximumDaysAhead,
        std::uint16_t startJitterMinutes)
    {
        std::vector<ScheduledSession> sessions;
        if (maximumSessions == 0 || maximumDaysAhead == 0 || weeklyPlan.dayMask == 0)
            return sessions;

        maximumSessions = std::min<std::uint32_t>(maximumSessions, 64);
        maximumDaysAhead = std::min<std::uint32_t>(maximumDaysAhead, 366);
        startDayIndex %= 7;

        for (std::uint32_t offset = 0; offset < maximumDaysAhead && sessions.size() < maximumSessions; ++offset)
        {
            std::uint32_t const absoluteDay = static_cast<std::uint32_t>(startDayIndex) + offset;
            std::uint32_t const weekIndex = startWeekIndex + absoluteDay / 7;
            std::uint8_t const dayIndex = static_cast<std::uint8_t>(absoluteDay % 7);

            if ((weeklyPlan.dayMask & (1U << dayIndex)) == 0)
                continue;

            ScheduleOccurrence occurrence = ScheduleOccurrencePlanner::Build(
                profileSeed,
                weeklyPlan,
                weekIndex,
                dayIndex,
                startJitterMinutes);

            sessions.push_back({ weekIndex, dayIndex, occurrence });
        }

        return sessions;
    }
}
