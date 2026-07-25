#include "LivingWorldPopulationRoleBalance.h"

#include <algorithm>

namespace LivingWorld
{
    namespace
    {
        std::uint32_t Total(PopulationRoleCounts const& counts)
        {
            return counts.tanks + counts.healers + counts.damage;
        }

        std::uint8_t Percent(std::uint32_t count, std::uint32_t total)
        {
            if (total == 0)
                return 0;
            return static_cast<std::uint8_t>((count * 100U) / total);
        }

        std::int16_t Adjustment(std::uint8_t currentPercent, std::uint8_t targetPercent, std::uint8_t tolerance)
        {
            std::int32_t const lower = std::max<std::int32_t>(0, static_cast<std::int32_t>(targetPercent) - tolerance);
            std::int32_t const upper = std::min<std::int32_t>(100, static_cast<std::int32_t>(targetPercent) + tolerance);

            if (currentPercent < lower)
            {
                std::int32_t const deficit = lower - currentPercent;
                return static_cast<std::int16_t>(std::min<std::int32_t>(
                    PopulationRoleBalancePolicy::MaximumPriorityAdjustment,
                    50 + deficit * 10));
            }

            if (currentPercent > upper)
            {
                std::int32_t const excess = currentPercent - upper;
                return static_cast<std::int16_t>(std::max<std::int32_t>(
                    -PopulationRoleBalancePolicy::MaximumPriorityAdjustment,
                    -50 - excess * 10));
            }

            return 0;
        }
    }

    std::int16_t PopulationRoleBalancePolicy::LoginPriorityAdjustment(
        PopulationRole role,
        PopulationRoleCounts const& online,
        PopulationRolePolicy policy)
    {
        policy.targetTankPercent = std::min<std::uint8_t>(policy.targetTankPercent, 100);
        policy.targetHealerPercent = std::min<std::uint8_t>(policy.targetHealerPercent, 100);
        policy.tolerancePercent = std::min<std::uint8_t>(policy.tolerancePercent, 50);

        if (static_cast<std::uint16_t>(policy.targetTankPercent) + policy.targetHealerPercent > 100)
            policy.targetHealerPercent = static_cast<std::uint8_t>(100 - policy.targetTankPercent);

        std::uint8_t const targetDamagePercent = static_cast<std::uint8_t>(
            100 - policy.targetTankPercent - policy.targetHealerPercent);
        std::uint32_t const total = Total(online);

        if (total == 0)
            return role == PopulationRole::Unknown ? 0 : 100;

        switch (role)
        {
            case PopulationRole::Tank:
                return Adjustment(Percent(online.tanks, total), policy.targetTankPercent, policy.tolerancePercent);
            case PopulationRole::Healer:
                return Adjustment(Percent(online.healers, total), policy.targetHealerPercent, policy.tolerancePercent);
            case PopulationRole::Damage:
                return Adjustment(Percent(online.damage, total), targetDamagePercent, policy.tolerancePercent);
            case PopulationRole::Unknown:
                return 0;
        }

        return 0;
    }
}
