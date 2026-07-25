#include "LivingWorldPopulationRoleBalance.h"

#include <cassert>
#include <iostream>

using namespace LivingWorld;

int main()
{
    PopulationRoleCounts balanced{ 20, 20, 60 };
    assert(PopulationRoleBalancePolicy::LoginPriorityAdjustment(PopulationRole::Tank, balanced) == 0);
    assert(PopulationRoleBalancePolicy::LoginPriorityAdjustment(PopulationRole::Healer, balanced) == 0);
    assert(PopulationRoleBalancePolicy::LoginPriorityAdjustment(PopulationRole::Damage, balanced) == 0);

    PopulationRoleCounts tankShortage{ 5, 20, 75 };
    assert(PopulationRoleBalancePolicy::LoginPriorityAdjustment(PopulationRole::Tank, tankShortage) > 0);
    assert(PopulationRoleBalancePolicy::LoginPriorityAdjustment(PopulationRole::Damage, tankShortage) < 0);

    PopulationRoleCounts healerShortage{ 20, 2, 78 };
    assert(PopulationRoleBalancePolicy::LoginPriorityAdjustment(PopulationRole::Healer, healerShortage) > 0);

    PopulationRoleCounts empty{};
    assert(PopulationRoleBalancePolicy::LoginPriorityAdjustment(PopulationRole::Tank, empty) == 100);
    assert(PopulationRoleBalancePolicy::LoginPriorityAdjustment(PopulationRole::Unknown, empty) == 0);

    PopulationRolePolicy invalid;
    invalid.targetTankPercent = 90;
    invalid.targetHealerPercent = 90;
    invalid.tolerancePercent = 100;
    std::int16_t const bounded = PopulationRoleBalancePolicy::LoginPriorityAdjustment(
        PopulationRole::Healer,
        tankShortage,
        invalid);
    assert(bounded >= -PopulationRoleBalancePolicy::MaximumPriorityAdjustment);
    assert(bounded <= PopulationRoleBalancePolicy::MaximumPriorityAdjustment);

    PopulationRoleCounts extreme{ 0, 0, 10000 };
    std::int16_t const tankBoost = PopulationRoleBalancePolicy::LoginPriorityAdjustment(PopulationRole::Tank, extreme);
    assert(tankBoost == PopulationRoleBalancePolicy::MaximumPriorityAdjustment);

    std::cout << "LivingWorld population role-balance tests passed.\n";
    return 0;
}
