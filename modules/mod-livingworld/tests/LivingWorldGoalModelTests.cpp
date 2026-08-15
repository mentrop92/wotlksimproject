#include "LivingWorldGoalModel.h"

#include <cassert>
#include <iostream>
#include <string>

using namespace LivingWorld;

int main()
{
    LivingWorldGoalModel model;

    LivingWorldGoal goal;
    goal.actorId = 1;
    goal.goalId = 100;
    goal.createdSimulationMinute = 10;
    goal.updatedSimulationMinute = 10;
    goal.priorityBasisPoints = 7500;
    goal.horizon = GoalHorizon::ShortTerm;
    goal.state = GoalState::Planned;
    goal.tag = "level_to_20";
    assert(model.AddGoal(goal));
    assert(model.Goals().size() == 1);

    auto found = model.FindGoal(1, 100);
    assert(found.has_value());
    assert(found->priorityBasisPoints == 7500);

    assert(model.UpdateGoalState(1, 100, GoalState::Active, 20));
    assert(model.UpdateGoalState(1, 100, GoalState::Completed, 30));
    assert(!model.UpdateGoalState(1, 100, GoalState::Active, 40));
    assert(!model.UpdateGoalState(1, 100, GoalState::Completed, 29));

    assert(!model.AddGoal(goal));

    LivingWorldGoal invalid = goal;
    invalid.goalId = 101;
    invalid.actorId = 0;
    assert(!model.AddGoal(invalid));
    invalid.actorId = 1;
    invalid.priorityBasisPoints = 10001;
    assert(!model.AddGoal(invalid));
    invalid.priorityBasisPoints = 100;
    invalid.tag = "unsafe tag";
    assert(!model.AddGoal(invalid));
    invalid.tag = std::string(LivingWorldGoalModel::MaximumTagBytes + 1, 'a');
    assert(!model.AddGoal(invalid));
    invalid.tag = "valid_goal";
    invalid.horizon = static_cast<GoalHorizon>(255);
    assert(!model.AddGoal(invalid));

    model.Reset();
    for (std::size_t i = 0; i < LivingWorldGoalModel::MaximumGoals; ++i)
    {
        LivingWorldGoal item;
        item.actorId = 10;
        item.goalId = 1000 + i;
        item.createdSimulationMinute = i;
        item.updatedSimulationMinute = i;
        item.priorityBasisPoints = 1000;
        item.horizon = static_cast<GoalHorizon>(i % 3);
        item.state = GoalState::Active;
        item.tag = "bounded_goal";
        assert(model.AddGoal(item));
    }
    assert(model.Goals().size() == LivingWorldGoalModel::MaximumGoals);

    LivingWorldGoal overflow;
    overflow.actorId = 10;
    overflow.goalId = 9999;
    overflow.createdSimulationMinute = 100;
    overflow.updatedSimulationMinute = 100;
    overflow.priorityBasisPoints = 1000;
    overflow.tag = "overflow_goal";
    assert(!model.AddGoal(overflow));

    assert(model.UpdateGoalState(10, 1000, GoalState::Completed, 200));
    assert(model.AddGoal(overflow));
    assert(model.Goals().size() == LivingWorldGoalModel::MaximumGoals);
    assert(!model.FindGoal(10, 1000).has_value());
    assert(model.FindGoal(10, 9999).has_value());

    std::cout << "LivingWorld bounded goal model tests passed.\n";
    return 0;
}
