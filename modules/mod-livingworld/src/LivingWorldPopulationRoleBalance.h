#ifndef MOD_LIVINGWORLD_POPULATION_ROLE_BALANCE_H
#define MOD_LIVINGWORLD_POPULATION_ROLE_BALANCE_H

#include <cstdint>

namespace LivingWorld
{
    enum class PopulationRole : std::uint8_t
    {
        Unknown = 0,
        Tank,
        Healer,
        Damage
    };

    struct PopulationRoleCounts
    {
        std::uint32_t tanks = 0;
        std::uint32_t healers = 0;
        std::uint32_t damage = 0;
    };

    struct PopulationRolePolicy
    {
        std::uint8_t targetTankPercent = 20;
        std::uint8_t targetHealerPercent = 20;
        std::uint8_t tolerancePercent = 5;
    };

    class PopulationRoleBalancePolicy
    {
    public:
        static constexpr std::int16_t MaximumPriorityAdjustment = 250;

        static std::int16_t LoginPriorityAdjustment(
            PopulationRole role,
            PopulationRoleCounts const& online,
            PopulationRolePolicy policy = {});
    };
}

#endif
