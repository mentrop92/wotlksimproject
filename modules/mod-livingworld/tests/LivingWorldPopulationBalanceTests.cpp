#include "LivingWorldPopulationBalance.h"

#include <cassert>
#include <iostream>

using namespace LivingWorld;

int main()
{
    PopulationBalanceSnapshot balanced{ 100, 100, 10 };
    assert(PopulationBalancePolicy::LoginPriorityAdjustment(PopulationFaction::Alliance, balanced) == 0);
    assert(PopulationBalancePolicy::LoginPriorityAdjustment(PopulationFaction::Horde, balanced) == 0);
    assert(PopulationBalancePolicy::LoginPriorityAdjustment(PopulationFaction::Neutral, balanced) == 0);

    PopulationBalanceSnapshot allianceHeavy{ 180, 20, 10 };
    assert(PopulationBalancePolicy::LoginPriorityAdjustment(PopulationFaction::Alliance, allianceHeavy) == -300);
    assert(PopulationBalancePolicy::LoginPriorityAdjustment(PopulationFaction::Horde, allianceHeavy) == 300);

    PopulationBalanceSnapshot modestImbalance{ 125, 75, 10 };
    assert(PopulationBalancePolicy::LoginPriorityAdjustment(PopulationFaction::Alliance, modestImbalance) == -40);
    assert(PopulationBalancePolicy::LoginPriorityAdjustment(PopulationFaction::Horde, modestImbalance) == 40);

    PopulationBalanceSnapshot invalidTolerance{ 190, 10, 100 };
    assert(PopulationBalancePolicy::LoginPriorityAdjustment(PopulationFaction::Alliance, invalidTolerance) == 0);

    PopulationBalanceSnapshot empty{};
    assert(PopulationBalancePolicy::LoginPriorityAdjustment(PopulationFaction::Alliance, empty) == 0);

    std::cout << "LivingWorld population balance tests passed.\n";
    return 0;
}
