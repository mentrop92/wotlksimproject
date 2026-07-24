#include "LivingWorldScheduleEligibility.h"

#include <algorithm>

namespace LivingWorld
{
    ScheduleEligibilityResult ScheduleEligibility::Evaluate(
        WeeklySchedulePlan const& plan,
        ScheduleEligibilityInput const& input,
        std::uint16_t earlyWindowMinutes,
        std::uint16_t lateWindowMinutes)
    {
        ScheduleEligibilityResult result;
        std::uint8_t const day = static_cast<std::uint8_t>(input.dayOfWeek % 7);
        result.scheduledToday = (plan.dayMask & (1U << day)) != 0;

        std::uint16_t const now = static_cast<std::uint16_t>(input.minuteOfDay % 1440);
        std::uint16_t const start = static_cast<std::uint16_t>(plan.preferredStartMinute % 1440);
        std::uint16_t const earliest = start >= earlyWindowMinutes
            ? static_cast<std::uint16_t>(start - earlyWindowMinutes)
            : 0;
        std::uint16_t const latest = static_cast<std::uint16_t>(std::min<std::uint32_t>(1439U, start + lateWindowMinutes));

        result.withinLoginWindow = result.scheduledToday && now >= earliest && now <= latest;
        if (result.scheduledToday && now < earliest)
            result.minutesUntilWindow = earliest - now;
        else if (!result.scheduledToday || now > latest)
            result.minutesUntilWindow = 1440U - now + earliest;

        result.eligible = result.withinLoginWindow && !input.alreadyOnline && !input.loginCooldownActive;
        return result;
    }
}
