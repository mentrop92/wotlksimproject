#include "LivingWorldGoalSelection.h"

#include <cassert>
#include <iostream>
#include <vector>

using namespace LivingWorld;

namespace
{
LivingWorldGoal Goal(std::uint64_t actor, std::uint64_t id, std::uint16_t priority, GoalHorizon horizon, GoalState state, std::uint64_t created)
{
    LivingWorldGoal goal;
    goal.actorId = actor;
    goal.goalId = id;
    goal.priorityBasisPoints = priority;
    goal.horizon = horizon;
    goal.state = state;
    goal.createdSimulationMinute = created;
    goal.updatedSimulationMinute = created;
    goal.tag = "goal";
    return goal;
}
}

int main()
{
    std::vector<LivingWorldGoal> goals = {
        Goal(1, 10, 6000, GoalHorizon::LongTerm, GoalState::Planned, 50),
        Goal(1, 11, 8000, GoalHorizon::MediumTerm, GoalState::Planned, 40),
        Goal(1, 12, 8000, GoalHorizon::ShortTerm, GoalState::Active, 30),
        Goal(2, 20, 10000, GoalHorizon::ShortTerm, GoalState::Active, 10),
        Goal(1, 13, 10000, GoalHorizon::ShortTerm, GoalState::Completed, 20)
    };

    GoalSelectionPolicy policy;
    policy.preferredHorizon = GoalHorizon::ShortTerm;
    auto selected = LivingWorldGoalSelection::SelectBest(goals, 1, policy);
    assert(selected.has_value() && selected->goalId == 12);

    policy.minimumPriorityBasisPoints = 9000;
    selected = LivingWorldGoalSelection::SelectBest(goals, 1, policy);
    assert(!selected.has_value());

    policy.minimumPriorityBasisPoints = 0;
    policy.allowActive = false;
    selected = LivingWorldGoalSelection::SelectBest(goals, 1, policy);
    assert(selected.has_value() && selected->goalId == 11);

    std::vector<LivingWorldGoal> tied = {
        Goal(3, 30, 7000, GoalHorizon::ShortTerm, GoalState::Planned, 100),
        Goal(3, 29, 7000, GoalHorizon::ShortTerm, GoalState::Planned, 100)
    };
    policy.allowActive = true;
    selected = LivingWorldGoalSelection::SelectBest(tied, 3, policy);
    assert(selected.has_value() && selected->goalId == 29);

    policy.minimumPriorityBasisPoints = LivingWorldGoalModel::MaximumPriorityBasisPoints + 1;
    assert(!LivingWorldGoalSelection::SelectBest(goals, 1, policy).has_value());
    assert(!LivingWorldGoalSelection::SelectBest(goals, 0, GoalSelectionPolicy{}).has_value());

    std::cout << "LivingWorld bounded goal selection tests passed.\n";
    return 0;
}
