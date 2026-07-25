#include "LivingWorldSchedulePlanner.h"

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

        std::uint16_t Range(std::uint64_t& state, std::uint16_t minimum, std::uint16_t maximum)
        {
            if (maximum <= minimum)
                return minimum;
            return static_cast<std::uint16_t>(minimum + (Next(state) % (maximum - minimum + 1)));
        }

        std::uint8_t DefaultDayMask(ActivityArchetype archetype)
        {
            switch (archetype)
            {
                case ActivityArchetype::Dormant: return 0b0100000;       // one occasional Saturday
                case ActivityArchetype::Casual: return 0b0101010;        // three scattered days
                case ActivityArchetype::Weekend: return 0b1100000;       // Saturday and Sunday
                case ActivityArchetype::Regular: return 0b1010111;       // five days
                case ActivityArchetype::Dedicated: return 0b1111110;     // six days
                case ActivityArchetype::Hardcore:
                case ActivityArchetype::NoLifeGrinder: return 0b1111111; // daily
            }
            return 0b0010101;
        }
    }

    WeeklySchedulePlan SchedulePlanner::Build(BotProfile const& profile)
    {
        std::uint64_t state = profile.seed ^ 0x6c6976696e67776fULL;
        WeeklySchedulePlan plan;
        plan.dayMask = DefaultDayMask(profile.lifestyle.archetype);

        // Most simulated players begin in an evening window. A minority are
        // morning, daytime, or overnight players, derived deterministically.
        std::uint32_t const windowRoll = Next(state) % 100;
        if (windowRoll < 10)
            plan.preferredStartMinute = Range(state, 360, 600);
        else if (windowRoll < 25)
            plan.preferredStartMinute = Range(state, 660, 960);
        else if (windowRoll < 90)
            plan.preferredStartMinute = Range(state, 1020, 1320);
        else
            plan.preferredStartMinute = Range(state, 0, 240);

        plan.plannedSessionMinutes = Range(
            state,
            profile.lifestyle.minimumSessionMinutes,
            profile.lifestyle.maximumSessionMinutes);
        plan.skipSessionChance = profile.lifestyle.skipSessionChance;
        plan.marathonEligible = (Next(state) % 100) < profile.lifestyle.marathonSessionChance;

        if (plan.marathonEligible)
        {
            std::uint32_t const extended = static_cast<std::uint32_t>(plan.plannedSessionMinutes) + 180U;
            plan.plannedSessionMinutes = static_cast<std::uint16_t>(std::min<std::uint32_t>(extended, 720U));
        }

        return plan;
    }
}
