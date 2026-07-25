#ifndef MOD_LIVINGWORLD_SCHEDULE_CALENDAR_H
#define MOD_LIVINGWORLD_SCHEDULE_CALENDAR_H

#include "LivingWorldScheduleOccurrence.h"

#include <cstdint>
#include <vector>

namespace LivingWorld
{
    struct ScheduledSession
    {
        std::uint32_t weekIndex = 0;
        std::uint8_t dayIndex = 0;
        ScheduleOccurrence occurrence;
    };

    class ScheduleCalendar
    {
    public:
        static std::vector<ScheduledSession> BuildUpcoming(
            std::uint64_t profileSeed,
            WeeklySchedulePlan const& weeklyPlan,
            std::uint32_t startWeekIndex,
            std::uint8_t startDayIndex,
            std::uint32_t maximumSessions,
            std::uint32_t maximumDaysAhead = 28,
            std::uint16_t startJitterMinutes = 30);
    };
}

#endif
