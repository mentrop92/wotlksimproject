#include "LivingWorldPopulationBalance.h"

#include <algorithm>
#include <cstdint>

namespace LivingWorld
{
    std::int16_t PopulationBalancePolicy::LoginPriorityAdjustment(
        PopulationFaction candidateFaction,
        PopulationBalanceSnapshot const& snapshot)
    {
        if (candidateFaction == PopulationFaction::Neutral)
            return 0;

        std::uint64_t const total = static_cast<std::uint64_t>(snapshot.allianceOnline) + snapshot.hordeOnline;
        if (total == 0)
            return 0;

        std::uint32_t const preferredImbalance = std::min<std::uint32_t>(snapshot.maximumPreferredImbalancePercent, 50);
        std::uint32_t const alliancePercent = static_cast<std::uint32_t>((static_cast<std::uint64_t>(snapshot.allianceOnline) * 100ULL) / total);
        std::uint32_t const hordePercent = 100U - alliancePercent;
        std::uint32_t const permittedMajority = 50U + preferredImbalance;

        std::uint32_t candidatePercent = candidateFaction == PopulationFaction::Alliance ? alliancePercent : hordePercent;
        std::uint32_t opposingPercent = candidateFaction == PopulationFaction::Alliance ? hordePercent : alliancePercent;

        if (candidatePercent > permittedMajority)
        {
            std::uint32_t const excess = candidatePercent - permittedMajority;
            return static_cast<std::int16_t>(-std::min<std::uint32_t>(excess * 20U, MaximumPriorityAdjustment));
        }

        if (opposingPercent > permittedMajority)
        {
            std::uint32_t const deficit = opposingPercent - permittedMajority;
            return static_cast<std::int16_t>(std::min<std::uint32_t>(deficit * 20U, MaximumPriorityAdjustment));
        }

        return 0;
    }
}
