#include "LivingWorldRelationshipPolicy.h"

#include <algorithm>

namespace LivingWorld
{
bool LivingWorldRelationshipPolicy::IsValidChoice(RelationshipChoice choice)
{
    switch (choice)
    {
        case RelationshipChoice::Invitation:
        case RelationshipChoice::Assistance:
        case RelationshipChoice::Trade:
        case RelationshipChoice::Guild:
            return true;
    }
    return false;
}

std::int32_t LivingWorldRelationshipPolicy::Score(LivingWorldSocialEdge const& relation)
{
    std::int32_t score = static_cast<std::int32_t>(relation.affinityBasisPoints) +
        static_cast<std::int32_t>(relation.trustBasisPoints) +
        static_cast<std::int32_t>(relation.familiarityBasisPoints) -
        static_cast<std::int32_t>(relation.rivalryBasisPoints);

    return std::clamp(score, MinimumScoreBasisPoints, MaximumScoreBasisPoints);
}

std::int32_t LivingWorldRelationshipPolicy::RequiredScore(RelationshipChoice choice)
{
    switch (choice)
    {
        case RelationshipChoice::Invitation:
            return 1500;
        case RelationshipChoice::Assistance:
            return 500;
        case RelationshipChoice::Trade:
            return 2500;
        case RelationshipChoice::Guild:
            return 6000;
    }
    return MaximumScoreBasisPoints;
}

bool LivingWorldRelationshipPolicy::MeetsHardGuards(
    LivingWorldSocialEdge const& relation,
    RelationshipChoice choice)
{
    if (relation.actorId == 0 || relation.otherId == 0 || relation.actorId == relation.otherId)
        return false;

    switch (choice)
    {
        case RelationshipChoice::Invitation:
            return relation.rivalryBasisPoints <= 6000;
        case RelationshipChoice::Assistance:
            return relation.rivalryBasisPoints <= 8000;
        case RelationshipChoice::Trade:
            return relation.trustBasisPoints >= 500 && relation.rivalryBasisPoints <= 5000;
        case RelationshipChoice::Guild:
            return relation.trustBasisPoints >= 2000 &&
                relation.familiarityBasisPoints >= 1500 &&
                relation.rivalryBasisPoints <= 3500;
    }
    return false;
}

RelationshipDecision LivingWorldRelationshipPolicy::Evaluate(
    LivingWorldSocialEdge const& relation,
    RelationshipChoice choice)
{
    RelationshipDecision decision;
    if (!IsValidChoice(choice))
        return decision;

    decision.scoreBasisPoints = Score(relation);
    decision.requiredScoreBasisPoints = RequiredScore(choice);
    decision.eligible = MeetsHardGuards(relation, choice) &&
        decision.scoreBasisPoints >= decision.requiredScoreBasisPoints;
    return decision;
}
}
