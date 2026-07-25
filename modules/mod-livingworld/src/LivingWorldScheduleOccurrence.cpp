#include "LivingWorldScheduleOccurrence.h"

#include <algorithm>

namespace LivingWorld
{
    namespace
    {
        std::uint32_t Next(std::uint64_t& state)
        {
            state ^= state >> 12;
            state ^= state << 25;
            state ^= state >> 27;
            return static_cast<std::uint32_t>((state * 2685821657736338717ULL) >> 32);
        }
    }

    ScheduleOccurrence ScheduleOccurrencePlanner::Build(
        std::uint64_t profileSeed,
        WeeklySchedulePlan const& weeklyPlan,
        std::uint32_t weekIndex,
        std::uint8_t dayIndex,
        std::uint16_t startJitterMinutes)
    {
        ScheduleOccurrence occurrence;

        if (dayIndex >= 7 || (weeklyPlan.dayMask & (1U << dayIndex)) == 0)
        {
            occurrence.skipped = true;
            return occurrence;
        }

        std::uint64_t state = profileSeed;
        state ^= (static_cast<std::uint64_t>(weekIndex) << 32);
        state ^= static_cast<std::uint64_t>(dayIndex) << 24;
        state ^= 0x73657373696f6e31ULL;

        occurrence.skipped = (Next(state) % 100U) < weeklyPlan.skipSessionChance;
        occurrence.marathon = !occurrence.skipped && weeklyPlan.marathonEligible && (Next(state) % 100U) < 35U;

        std::uint16_t const boundedJitter = std::min<std::uint16_t>(startJitterMinutes, 180);
        std::int32_t jitter = 0;
        if (boundedJitter > 0)
        {
            std::uint32_t const width = static_cast<std::uint32_t>(boundedJitter) * 2U + 1U;
            jitter = static_cast<std::int32_t>(Next(state) % width) - boundedJitter;
        }

        std::int32_t start = static_cast<std::int32_t>(weeklyPlan.preferredStartMinute) + jitter;
        while (start < 0)
            start += 1440;
        occurrence.startMinute = static_cast<std::uint16_t>(start % 1440);

        std::uint32_t duration = weeklyPlan.plannedSessionMinutes;
        std::int32_t durationVariation = static_cast<std::int32_t>(Next(state) % 41U) - 20;
        duration = static_cast<std::uint32_t>(std::max<std::int32_t>(15, static_cast<std::int32_t>(duration) + durationVariation));
        if (occurrence.marathon)
            duration = std::min<std::uint32_t>(duration + 180U, 720U);
        occurrence.sessionMinutes = static_cast<std::uint16_t>(std::min<std::uint32_t>(duration, 720U));

        return occurrence;
    }
}
