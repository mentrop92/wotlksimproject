#include "LivingWorldScheduleEligibility.h"

#include <algorithm>
#include <limits>

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
        if (result.withinLoginWindow)
        {
            result.minutesUntilWindow = 0;
        }
        else
        {
            std::uint32_t nextWindow = std::numeric_limits<std::uint32_t>::max();
            for (std::uint32_t dayOffset = 0; dayOffset <= 7; ++dayOffset)
            {
                std::uint8_t const candidateDay = static_cast<std::uint8_t>((day + dayOffset) % 7);
                if ((plan.dayMask & (1U << candidateDay)) == 0)
                    continue;

                std::int64_t minutes = static_cast<std::int64_t>(dayOffset) * 1440LL + earliest - now;
                if (minutes <= 0)
                    continue;

                nextWindow = std::min(nextWindow, static_cast<std::uint32_t>(minutes));
            }

            // A zero day mask is invalid but must remain safe and deterministic.
            result.minutesUntilWindow = nextWindow == std::numeric_limits<std::uint32_t>::max()
                ? 7U * 1440U
                : nextWindow;
        }

        result.eligible = result.withinLoginWindow && !input.alreadyOnline && !input.loginCooldownActive;
        return result;
    }
}
