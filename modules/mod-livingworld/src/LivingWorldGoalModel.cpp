#include "LivingWorldGoalModel.h"

#include <algorithm>
#include <cctype>

namespace LivingWorld
{
bool LivingWorldGoalModel::IsSafeTag(std::string const& tag)
{
    if (tag.empty() || tag.size() > MaximumTagBytes)
        return false;

    return std::all_of(tag.begin(), tag.end(), [](unsigned char ch)
    {
        return std::isalnum(ch) || ch == '_' || ch == '-' || ch == '.';
    });
}

bool LivingWorldGoalModel::IsValidHorizon(GoalHorizon horizon)
{
    switch (horizon)
    {
        case GoalHorizon::ShortTerm:
        case GoalHorizon::MediumTerm:
        case GoalHorizon::LongTerm:
            return true;
    }
    return false;
}

bool LivingWorldGoalModel::IsValidState(GoalState state)
{
    switch (state)
    {
        case GoalState::Planned:
        case GoalState::Active:
        case GoalState::Completed:
        case GoalState::Abandoned:
            return true;
    }
    return false;
}

bool LivingWorldGoalModel::IsTerminal(GoalState state)
{
    return state == GoalState::Completed || state == GoalState::Abandoned;
}

bool LivingWorldGoalModel::IsValidTransition(GoalState from, GoalState to)
{
    if (!IsValidState(from) || !IsValidState(to) || IsTerminal(from))
        return false;
    if (from == to)
        return true;
    if (from == GoalState::Planned)
        return to == GoalState::Active || to == GoalState::Abandoned;
    if (from == GoalState::Active)
        return to == GoalState::Completed || to == GoalState::Abandoned;
    return false;
}

bool LivingWorldGoalModel::AddGoal(LivingWorldGoal goal)
{
    if (goal.actorId == 0 || goal.goalId == 0)
        return false;
    if (!IsValidHorizon(goal.horizon) || !IsValidState(goal.state))
        return false;
    if (goal.priorityBasisPoints > MaximumPriorityBasisPoints || !IsSafeTag(goal.tag))
        return false;
    if (goal.updatedSimulationMinute < goal.createdSimulationMinute)
        return false;
    if (FindGoal(goal.actorId, goal.goalId).has_value())
        return false;

    if (goals_.size() >= MaximumGoals)
    {
        auto evict = std::find_if(goals_.begin(), goals_.end(), [](LivingWorldGoal const& candidate)
        {
            return IsTerminal(candidate.state);
        });
        if (evict == goals_.end())
            return false;
        goals_.erase(evict);
    }

    goals_.push_back(std::move(goal));
    return true;
}

bool LivingWorldGoalModel::UpdateGoalState(
    std::uint64_t actorId,
    std::uint64_t goalId,
    GoalState newState,
    std::uint64_t simulationMinute)
{
    auto found = std::find_if(goals_.begin(), goals_.end(), [actorId, goalId](LivingWorldGoal const& goal)
    {
        return goal.actorId == actorId && goal.goalId == goalId;
    });
    if (found == goals_.end() || simulationMinute < found->updatedSimulationMinute)
        return false;
    if (!IsValidTransition(found->state, newState))
        return false;

    found->state = newState;
    found->updatedSimulationMinute = simulationMinute;
    return true;
}

std::optional<LivingWorldGoal> LivingWorldGoalModel::FindGoal(std::uint64_t actorId, std::uint64_t goalId) const
{
    auto found = std::find_if(goals_.begin(), goals_.end(), [actorId, goalId](LivingWorldGoal const& goal)
    {
        return goal.actorId == actorId && goal.goalId == goalId;
    });
    if (found == goals_.end())
        return std::nullopt;
    return *found;
}

std::vector<LivingWorldGoal> const& LivingWorldGoalModel::Goals() const
{
    return goals_;
}

void LivingWorldGoalModel::Reset()
{
    goals_.clear();
}
}
