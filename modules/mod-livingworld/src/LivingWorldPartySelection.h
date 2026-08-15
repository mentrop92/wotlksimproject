#ifndef MOD_LIVINGWORLD_PARTY_SELECTION_H
#define MOD_LIVINGWORLD_PARTY_SELECTION_H

#include "LivingWorldMemorySocialGraph.h"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace LivingWorld
{
enum class PartyRole : std::uint8_t
{
    Tank = 0,
    Healer = 1,
    Damage = 2
};

struct LivingWorldPartyCandidate
{
    std::uint64_t characterId = 0;
    PartyRole role = PartyRole::Damage;
    bool available = false;
};

struct PartySelectionPolicy
{
    std::uint8_t desiredPartySize = 5;
    std::int16_t minimumAffinityBasisPoints = -10000;
    std::uint16_t minimumTrustBasisPoints = 0;
    std::uint16_t minimumFamiliarityBasisPoints = 0;
};

struct LivingWorldPartySelectionResult
{
    std::uint64_t initiatorId = 0;
    std::vector<std::uint64_t> selectedCharacterIds;
};

class LivingWorldPartySelection
{
public:
    static constexpr std::size_t MaximumCandidates = 64;
    static constexpr std::uint8_t MinimumPartySize = 2;
    static constexpr std::uint8_t MaximumPartySize = 5;

    static std::optional<LivingWorldPartySelectionResult> Select(
        std::uint64_t initiatorId,
        std::vector<LivingWorldPartyCandidate> const& candidates,
        std::vector<LivingWorldSocialEdge> const& relations,
        PartySelectionPolicy const& policy);

private:
    static bool IsValidRole(PartyRole role);
};
}

#endif
