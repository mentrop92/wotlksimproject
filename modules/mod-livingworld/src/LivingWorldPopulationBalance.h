#ifndef MOD_LIVINGWORLD_POPULATION_BALANCE_H
#define MOD_LIVINGWORLD_POPULATION_BALANCE_H

#include <cstdint>

namespace LivingWorld
{
    enum class PopulationFaction : std::uint8_t
    {
        Neutral = 0,
        Alliance,
        Horde
    };

    struct PopulationBalanceSnapshot
    {
        std::uint32_t allianceOnline = 0;
        std::uint32_t hordeOnline = 0;
        std::uint8_t maximumPreferredImbalancePercent = 10;
    };

    class PopulationBalancePolicy
    {
    public:
        static constexpr std::int16_t MaximumPriorityAdjustment = 300;

        static std::int16_t LoginPriorityAdjustment(
            PopulationFaction candidateFaction,
            PopulationBalanceSnapshot const& snapshot);
    };
}

#endif
