#ifndef MOD_LIVINGWORLD_SCHEDULE_OCCURRENCE_H
#define MOD_LIVINGWORLD_SCHEDULE_OCCURRENCE_H

#include "LivingWorldSchedulePlanner.h"

#include <cstdint>

namespace LivingWorld
{
    struct ScheduleOccurrence
    {
        bool skipped = false;
        bool marathon = false;
        std::uint16_t startMinute = 0;
        std::uint16_t sessionMinutes = 0;
    };

    class ScheduleOccurrencePlanner
    {
    public:
        static ScheduleOccurrence Build(
            std::uint64_t profileSeed,
            WeeklySchedulePlan const& weeklyPlan,
            std::uint32_t weekIndex,
            std::uint8_t dayIndex,
            std::uint16_t startJitterMinutes = 30);
    };
}

#endif
