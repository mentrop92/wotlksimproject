#include "LivingWorldGoalSelection.h"

#include <tuple>

namespace LivingWorld
{
namespace
{
bool IsEligibleState(GoalState state, GoalSelectionPolicy const& policy)
{
    if (state == GoalState::Planned)
        return policy.allowPlanned;
    if (state == GoalState::Active)
        return policy.allowActive;
    return false;
}

std::uint8_t HorizonDistance(GoalHorizon lhs, GoalHorizon rhs)
{
    auto a = static_cast<std::uint8_t>(lhs);
    auto b = static_cast<std::uint8_t>(rhs);
    return a > b ? a - b : b - a;
}
}

std::optional<LivingWorldGoal> LivingWorldGoalSelection::SelectBest(
    std::vector<LivingWorldGoal> const& goals,
    std::uint64_t actorId,
    GoalSelectionPolicy const& policy)
{
    if (actorId == 0 || policy.minimumPriorityBasisPoints > LivingWorldGoalModel::MaximumPriorityBasisPoints)
        return std::nullopt;

    std::optional<LivingWorldGoal> best;
    for (LivingWorldGoal const& goal : goals)
    {
        if (goal.actorId != actorId || goal.priorityBasisPoints < policy.minimumPriorityBasisPoints || !IsEligibleState(goal.state, policy))
            continue;

        if (!best.has_value())
        {
            best = goal;
            continue;
        }

        auto candidateKey = std::make_tuple(
            goal.priorityBasisPoints,
            static_cast<std::uint8_t>(255 - HorizonDistance(goal.horizon, policy.preferredHorizon)),
            static_cast<std::uint8_t>(goal.state == GoalState::Active ? 1 : 0),
            static_cast<std::uint64_t>(~goal.createdSimulationMinute),
            static_cast<std::uint64_t>(~goal.goalId));
        auto bestKey = std::make_tuple(
            best->priorityBasisPoints,
            static_cast<std::uint8_t>(255 - HorizonDistance(best->horizon, policy.preferredHorizon)),
            static_cast<std::uint8_t>(best->state == GoalState::Active ? 1 : 0),
            static_cast<std::uint64_t>(~best->createdSimulationMinute),
            static_cast<std::uint64_t>(~best->goalId));

        if (candidateKey > bestKey)
            best = goal;
    }

    return best;
}
}
