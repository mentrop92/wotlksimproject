#ifndef MOD_LIVINGWORLD_SCHEDULE_ELIGIBILITY_H
#define MOD_LIVINGWORLD_SCHEDULE_ELIGIBILITY_H

#include "LivingWorldSchedulePlanner.h"

#include <cstdint>

namespace LivingWorld
{
    struct ScheduleEligibilityInput
    {
        std::uint8_t dayOfWeek = 0;
        std::uint16_t minuteOfDay = 0;
        std::uint32_t minutesSinceLastLogout = 0;
        bool alreadyOnline = false;
        bool loginCooldownActive = false;
    };

    struct ScheduleEligibilityResult
    {
        bool scheduledToday = false;
        bool withinLoginWindow = false;
        bool eligible = false;
        std::uint32_t minutesUntilWindow = 0;
    };

    class ScheduleEligibility
    {
    public:
        static ScheduleEligibilityResult Evaluate(
            WeeklySchedulePlan const& plan,
            ScheduleEligibilityInput const& input,
            std::uint16_t earlyWindowMinutes = 15,
            std::uint16_t lateWindowMinutes = 90);
    };
}

#endif
