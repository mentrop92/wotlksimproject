#include "LivingWorldPartySelection.h"

#include <algorithm>
#include <cstdint>
#include <set>
#include <utility>
#include <vector>

namespace LivingWorld
{
namespace
{
struct RankedCandidate
{
    std::uint64_t characterId = 0;
    std::int32_t score = 0;
    std::uint16_t trustBasisPoints = 0;
    std::uint16_t familiarityBasisPoints = 0;
    std::uint16_t rivalryBasisPoints = 0;
};
}

bool LivingWorldPartySelection::IsValidRole(PartyRole role)
{
    switch (role)
    {
        case PartyRole::Tank:
        case PartyRole::Healer:
        case PartyRole::Damage:
            return true;
    }
    return false;
}

std::optional<LivingWorldPartySelectionResult> LivingWorldPartySelection::Select(
    std::uint64_t initiatorId,
    std::vector<LivingWorldPartyCandidate> const& candidates,
    std::vector<LivingWorldSocialEdge> const& relations,
    PartySelectionPolicy const& policy)
{
    if (initiatorId == 0 || candidates.size() > MaximumCandidates ||
        relations.size() > LivingWorldMemorySocialGraph::MaximumRelations)
        return std::nullopt;
    if (policy.desiredPartySize < MinimumPartySize || policy.desiredPartySize > MaximumPartySize)
        return std::nullopt;
    if (policy.minimumAffinityBasisPoints < LivingWorldMemorySocialGraph::MinimumSignedBasisPoints ||
        policy.minimumAffinityBasisPoints > LivingWorldMemorySocialGraph::MaximumSignedBasisPoints ||
        policy.minimumTrustBasisPoints > LivingWorldMemorySocialGraph::MaximumUnsignedBasisPoints ||
        policy.minimumFamiliarityBasisPoints > LivingWorldMemorySocialGraph::MaximumUnsignedBasisPoints)
        return std::nullopt;

    std::set<std::pair<std::uint64_t, std::uint64_t>> relationKeys;
    for (LivingWorldSocialEdge const& edge : relations)
    {
        if (edge.actorId == 0 || edge.otherId == 0 || edge.actorId == edge.otherId ||
            edge.affinityBasisPoints < LivingWorldMemorySocialGraph::MinimumSignedBasisPoints ||
            edge.affinityBasisPoints > LivingWorldMemorySocialGraph::MaximumSignedBasisPoints ||
            edge.trustBasisPoints > LivingWorldMemorySocialGraph::MaximumUnsignedBasisPoints ||
            edge.familiarityBasisPoints > LivingWorldMemorySocialGraph::MaximumUnsignedBasisPoints ||
            edge.rivalryBasisPoints > LivingWorldMemorySocialGraph::MaximumUnsignedBasisPoints)
            return std::nullopt;
        if (!relationKeys.insert({edge.actorId, edge.otherId}).second)
            return std::nullopt;
    }

    std::set<std::uint64_t> seen;
    std::vector<RankedCandidate> ranked;
    ranked.reserve(candidates.size());

    for (LivingWorldPartyCandidate const& candidate : candidates)
    {
        if (candidate.characterId == 0 || candidate.characterId == initiatorId || !IsValidRole(candidate.role))
            return std::nullopt;
        if (!seen.insert(candidate.characterId).second)
            return std::nullopt;
        if (!candidate.available)
            continue;

        auto relation = std::find_if(relations.begin(), relations.end(), [&](LivingWorldSocialEdge const& edge)
        {
            return edge.actorId == initiatorId && edge.otherId == candidate.characterId;
        });
        if (relation == relations.end())
            continue;
        if (relation->affinityBasisPoints < policy.minimumAffinityBasisPoints ||
            relation->trustBasisPoints < policy.minimumTrustBasisPoints ||
            relation->familiarityBasisPoints < policy.minimumFamiliarityBasisPoints)
            continue;

        RankedCandidate item;
        item.characterId = candidate.characterId;
        item.trustBasisPoints = relation->trustBasisPoints;
        item.familiarityBasisPoints = relation->familiarityBasisPoints;
        item.rivalryBasisPoints = relation->rivalryBasisPoints;
        item.score = static_cast<std::int32_t>(relation->affinityBasisPoints) +
            static_cast<std::int32_t>(relation->trustBasisPoints) +
            static_cast<std::int32_t>(relation->familiarityBasisPoints) -
            static_cast<std::int32_t>(relation->rivalryBasisPoints);
        ranked.push_back(item);
    }

    std::sort(ranked.begin(), ranked.end(), [](RankedCandidate const& lhs, RankedCandidate const& rhs)
    {
        if (lhs.score != rhs.score)
            return lhs.score > rhs.score;
        if (lhs.trustBasisPoints != rhs.trustBasisPoints)
            return lhs.trustBasisPoints > rhs.trustBasisPoints;
        if (lhs.familiarityBasisPoints != rhs.familiarityBasisPoints)
            return lhs.familiarityBasisPoints > rhs.familiarityBasisPoints;
        if (lhs.rivalryBasisPoints != rhs.rivalryBasisPoints)
            return lhs.rivalryBasisPoints < rhs.rivalryBasisPoints;
        return lhs.characterId < rhs.characterId;
    });

    LivingWorldPartySelectionResult result;
    result.initiatorId = initiatorId;
    std::size_t slots = static_cast<std::size_t>(policy.desiredPartySize - 1);
    for (std::size_t i = 0; i < ranked.size() && i < slots; ++i)
        result.selectedCharacterIds.push_back(ranked[i].characterId);

    return result;
}
}
