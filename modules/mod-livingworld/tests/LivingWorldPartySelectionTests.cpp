#include "LivingWorldPartySelection.h"

#include <cassert>
#include <iostream>
#include <vector>

using namespace LivingWorld;

namespace
{
LivingWorldSocialEdge Relation(
    std::uint64_t actor,
    std::uint64_t other,
    std::int16_t affinity,
    std::uint16_t trust,
    std::uint16_t familiarity,
    std::uint16_t rivalry)
{
    LivingWorldSocialEdge edge;
    edge.actorId = actor;
    edge.otherId = other;
    edge.affinityBasisPoints = affinity;
    edge.trustBasisPoints = trust;
    edge.familiarityBasisPoints = familiarity;
    edge.rivalryBasisPoints = rivalry;
    return edge;
}

LivingWorldPartyCandidate Candidate(std::uint64_t id, bool available = true, PartyRole role = PartyRole::Damage)
{
    LivingWorldPartyCandidate candidate;
    candidate.characterId = id;
    candidate.available = available;
    candidate.role = role;
    return candidate;
}
}

int main()
{
    std::vector<LivingWorldPartyCandidate> candidates = {
        Candidate(2), Candidate(3), Candidate(4), Candidate(5), Candidate(6, false)
    };
    std::vector<LivingWorldSocialEdge> relations = {
        Relation(1, 2, 2000, 7000, 5000, 500),
        Relation(1, 3, 4000, 6000, 5500, 200),
        Relation(1, 4, -1000, 9000, 8000, 0),
        Relation(1, 5, 2000, 7000, 5000, 500),
        Relation(1, 6, 10000, 10000, 10000, 0)
    };

    PartySelectionPolicy policy;
    policy.desiredPartySize = 4;
    auto result = LivingWorldPartySelection::Select(1, candidates, relations, policy);
    assert(result.has_value());
    assert(result->initiatorId == 1);
    assert(result->selectedCharacterIds.size() == 3);
    assert(result->selectedCharacterIds[0] == 4);
    assert(result->selectedCharacterIds[1] == 3);
    assert(result->selectedCharacterIds[2] == 2);

    policy.minimumAffinityBasisPoints = 0;
    policy.minimumTrustBasisPoints = 6500;
    result = LivingWorldPartySelection::Select(1, candidates, relations, policy);
    assert(result.has_value());
    assert(result->selectedCharacterIds.size() == 2);
    assert(result->selectedCharacterIds[0] == 2);
    assert(result->selectedCharacterIds[1] == 5);

    policy.minimumAffinityBasisPoints = -10000;
    policy.minimumTrustBasisPoints = 0;
    policy.desiredPartySize = 2;
    result = LivingWorldPartySelection::Select(1, candidates, relations, policy);
    assert(result.has_value());
    assert(result->selectedCharacterIds.size() == 1 && result->selectedCharacterIds[0] == 4);

    assert(!LivingWorldPartySelection::Select(0, candidates, relations, policy).has_value());
    policy.desiredPartySize = 1;
    assert(!LivingWorldPartySelection::Select(1, candidates, relations, policy).has_value());
    policy.desiredPartySize = 6;
    assert(!LivingWorldPartySelection::Select(1, candidates, relations, policy).has_value());

    policy.desiredPartySize = 5;
    auto duplicateCandidates = candidates;
    duplicateCandidates.push_back(Candidate(2));
    assert(!LivingWorldPartySelection::Select(1, duplicateCandidates, relations, policy).has_value());

    auto invalidCandidates = candidates;
    invalidCandidates[0].role = static_cast<PartyRole>(255);
    assert(!LivingWorldPartySelection::Select(1, invalidCandidates, relations, policy).has_value());

    std::vector<LivingWorldPartyCandidate> tooMany;
    for (std::size_t i = 0; i <= LivingWorldPartySelection::MaximumCandidates; ++i)
        tooMany.push_back(Candidate(100 + i));
    assert(!LivingWorldPartySelection::Select(1, tooMany, relations, policy).has_value());

    auto duplicateRelations = relations;
    duplicateRelations.push_back(relations.front());
    assert(!LivingWorldPartySelection::Select(1, candidates, duplicateRelations, policy).has_value());

    auto invalidRelations = relations;
    invalidRelations.front().trustBasisPoints = LivingWorldMemorySocialGraph::MaximumUnsignedBasisPoints + 1;
    assert(!LivingWorldPartySelection::Select(1, candidates, invalidRelations, policy).has_value());

    invalidRelations = relations;
    invalidRelations.front().actorId = invalidRelations.front().otherId;
    assert(!LivingWorldPartySelection::Select(1, candidates, invalidRelations, policy).has_value());

    std::vector<LivingWorldSocialEdge> tooManyRelations;
    for (std::size_t i = 0; i <= LivingWorldMemorySocialGraph::MaximumRelations; ++i)
        tooManyRelations.push_back(Relation(1000 + i, 2000 + i, 0, 0, 0, 0));
    assert(!LivingWorldPartySelection::Select(1, candidates, tooManyRelations, policy).has_value());

    std::cout << "LivingWorld bounded party selection tests passed.\n";
    return 0;
}
