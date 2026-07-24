#ifndef MOD_LIVINGWORLD_POPULATION_COOLDOWNS_H
#define MOD_LIVINGWORLD_POPULATION_COOLDOWNS_H

#include "LivingWorldPopulationController.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace LivingWorld
{
    namespace PopulationCooldownLimits
    {
        inline constexpr std::uint32_t MaximumCooldownMs = 24U * 60U * 60U * 1000U;
    }

    class PopulationCooldowns
    {
    public:
        void Reset();
        void Advance(std::uint32_t diffMs);
        void Mark(std::uint32_t characterGuid, PopulationAction action, std::uint32_t cooldownMs);
        bool IsEligible(std::uint32_t characterGuid, PopulationAction action) const;
        std::uint32_t RemainingMs(std::uint32_t characterGuid, PopulationAction action) const;
        std::vector<std::uint32_t> FilterEligible(
            std::vector<std::uint32_t> const& characterGuids,
            PopulationAction action,
            std::uint32_t limit) const;

    private:
        using CooldownMap = std::unordered_map<std::uint32_t, std::uint32_t>;

        static void AdvanceMap(CooldownMap& cooldowns, std::uint32_t diffMs);
        CooldownMap& MapFor(PopulationAction action);
        CooldownMap const& MapFor(PopulationAction action) const;

        CooldownMap _loginCooldowns;
        CooldownMap _logoutCooldowns;
    };
}

#endif
