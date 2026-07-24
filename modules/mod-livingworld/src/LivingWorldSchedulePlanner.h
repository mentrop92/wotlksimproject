#ifndef MOD_LIVINGWORLD_SCHEDULE_PLANNER_H
#define MOD_LIVINGWORLD_SCHEDULE_PLANNER_H

#include "LivingWorldProfile.h"

#include <cstdint>

namespace LivingWorld
{
    struct WeeklySchedulePlan
    {
        std::uint8_t dayMask = 0;
        std::uint16_t preferredStartMinute = 0;
        std::uint16_t plannedSessionMinutes = 0;
        std::uint8_t skipSessionChance = 0;
        bool marathonEligible = false;
    };

    class SchedulePlanner
    {
    public:
        static WeeklySchedulePlan Build(BotProfile const& profile);
    };
}

#endif
