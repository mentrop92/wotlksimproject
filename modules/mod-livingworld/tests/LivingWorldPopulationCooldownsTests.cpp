#include "LivingWorldPopulationCooldowns.h"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

using namespace LivingWorld;

int main()
{
    PopulationCooldowns cooldowns;

    cooldowns.Mark(10, PopulationAction::Login, 60000);
    assert(!cooldowns.IsEligible(10, PopulationAction::Login));
    assert(cooldowns.IsEligible(10, PopulationAction::Logout));
    assert(cooldowns.RemainingMs(10, PopulationAction::Login) == 60000);

    cooldowns.Advance(15000);
    assert(cooldowns.RemainingMs(10, PopulationAction::Login) == 45000);

    cooldowns.Advance(45000);
    assert(cooldowns.IsEligible(10, PopulationAction::Login));

    cooldowns.Mark(20, PopulationAction::Logout, PopulationCooldownLimits::MaximumCooldownMs + 5000U);
    assert(cooldowns.RemainingMs(20, PopulationAction::Logout) == PopulationCooldownLimits::MaximumCooldownMs);

    cooldowns.Mark(30, PopulationAction::Logout, 30000);
    std::vector<std::uint32_t> const candidates = { 20, 30, 40, 50 };
    std::vector<std::uint32_t> const filtered = cooldowns.FilterEligible(
        candidates,
        PopulationAction::Logout,
        2);
    assert(filtered.size() == 2);
    assert(filtered[0] == 40);
    assert(filtered[1] == 50);

    cooldowns.Mark(30, PopulationAction::Logout, 0);
    assert(cooldowns.IsEligible(30, PopulationAction::Logout));

    cooldowns.Reset();
    assert(cooldowns.IsEligible(20, PopulationAction::Logout));

    std::cout << "LivingWorld population cooldown tests passed.\n";
    return 0;
}
