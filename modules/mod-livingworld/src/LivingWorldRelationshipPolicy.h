#ifndef MOD_LIVINGWORLD_RELATIONSHIP_POLICY_H
#define MOD_LIVINGWORLD_RELATIONSHIP_POLICY_H

#include "LivingWorldMemorySocialGraph.h"

#include <cstdint>

namespace LivingWorld
{
enum class RelationshipChoice : std::uint8_t
{
    Invitation = 0,
    Assistance = 1,
    Trade = 2,
    Guild = 3
};

struct RelationshipDecision
{
    bool eligible = false;
    std::int32_t scoreBasisPoints = 0;
    std::int32_t requiredScoreBasisPoints = 0;
};

class LivingWorldRelationshipPolicy
{
public:
    static constexpr std::int32_t MinimumScoreBasisPoints = -20000;
    static constexpr std::int32_t MaximumScoreBasisPoints = 30000;

    static RelationshipDecision Evaluate(
        LivingWorldSocialEdge const& relation,
        RelationshipChoice choice);

private:
    static bool IsValidChoice(RelationshipChoice choice);
    static std::int32_t Score(LivingWorldSocialEdge const& relation);
    static std::int32_t RequiredScore(RelationshipChoice choice);
    static bool MeetsHardGuards(LivingWorldSocialEdge const& relation, RelationshipChoice choice);
};
}

#endif
