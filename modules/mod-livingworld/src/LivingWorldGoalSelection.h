#ifndef MOD_LIVINGWORLD_GOAL_SELECTION_H
#define MOD_LIVINGWORLD_GOAL_SELECTION_H

#include "LivingWorldGoalModel.h"

#include <cstdint>
#include <optional>
#include <vector>

namespace LivingWorld
{
struct GoalSelectionPolicy
{
    GoalHorizon preferredHorizon = GoalHorizon::ShortTerm;
    std::uint16_t minimumPriorityBasisPoints = 0;
    bool allowPlanned = true;
    bool allowActive = true;
};

class LivingWorldGoalSelection
{
public:
    static std::optional<LivingWorldGoal> SelectBest(
        std::vector<LivingWorldGoal> const& goals,
        std::uint64_t actorId,
        GoalSelectionPolicy const& policy);
};
}

#endif
