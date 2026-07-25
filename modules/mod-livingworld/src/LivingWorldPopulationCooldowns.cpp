#include "LivingWorldPopulationCooldowns.h"

#include <algorithm>

namespace LivingWorld
{
    void PopulationCooldowns::Reset()
    {
        _loginCooldowns.clear();
        _logoutCooldowns.clear();
    }

    void PopulationCooldowns::Advance(std::uint32_t diffMs)
    {
        AdvanceMap(_loginCooldowns, diffMs);
        AdvanceMap(_logoutCooldowns, diffMs);
    }

    void PopulationCooldowns::Mark(
        std::uint32_t characterGuid,
        PopulationAction action,
        std::uint32_t cooldownMs)
    {
        if (characterGuid == 0 || action == PopulationAction::None)
            return;

        std::uint32_t const bounded = std::min(cooldownMs, PopulationCooldownLimits::MaximumCooldownMs);
        if (bounded == 0)
        {
            MapFor(action).erase(characterGuid);
            return;
        }

        MapFor(action)[characterGuid] = bounded;
    }

    bool PopulationCooldowns::IsEligible(std::uint32_t characterGuid, PopulationAction action) const
    {
        return RemainingMs(characterGuid, action) == 0;
    }

    std::uint32_t PopulationCooldowns::RemainingMs(
        std::uint32_t characterGuid,
        PopulationAction action) const
    {
        if (characterGuid == 0 || action == PopulationAction::None)
            return 0;

        CooldownMap const& cooldowns = MapFor(action);
        auto const itr = cooldowns.find(characterGuid);
        return itr == cooldowns.end() ? 0 : itr->second;
    }

    std::vector<std::uint32_t> PopulationCooldowns::FilterEligible(
        std::vector<std::uint32_t> const& characterGuids,
        PopulationAction action,
        std::uint32_t limit) const
    {
        std::vector<std::uint32_t> eligible;
        eligible.reserve(std::min<std::size_t>(characterGuids.size(), limit));

        for (std::uint32_t characterGuid : characterGuids)
        {
            if (!IsEligible(characterGuid, action))
                continue;

            eligible.push_back(characterGuid);
            if (eligible.size() >= limit)
                break;
        }

        return eligible;
    }

    void PopulationCooldowns::AdvanceMap(CooldownMap& cooldowns, std::uint32_t diffMs)
    {
        for (auto itr = cooldowns.begin(); itr != cooldowns.end();)
        {
            if (diffMs >= itr->second)
                itr = cooldowns.erase(itr);
            else
            {
                itr->second -= diffMs;
                ++itr;
            }
        }
    }

    PopulationCooldowns::CooldownMap& PopulationCooldowns::MapFor(PopulationAction action)
    {
        return action == PopulationAction::Login ? _loginCooldowns : _logoutCooldowns;
    }

    PopulationCooldowns::CooldownMap const& PopulationCooldowns::MapFor(PopulationAction action) const
    {
        return action == PopulationAction::Login ? _loginCooldowns : _logoutCooldowns;
    }
}
