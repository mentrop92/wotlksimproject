#ifndef MOD_LIVINGWORLD_GOAL_MODEL_H
#define MOD_LIVINGWORLD_GOAL_MODEL_H

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace LivingWorld
{
enum class GoalHorizon : std::uint8_t
{
    ShortTerm = 0,
    MediumTerm = 1,
    LongTerm = 2
};

enum class GoalState : std::uint8_t
{
    Planned = 0,
    Active = 1,
    Completed = 2,
    Abandoned = 3
};

struct LivingWorldGoal
{
    std::uint64_t actorId = 0;
    std::uint64_t goalId = 0;
    std::uint64_t createdSimulationMinute = 0;
    std::uint64_t updatedSimulationMinute = 0;
    std::uint16_t priorityBasisPoints = 0;
    GoalHorizon horizon = GoalHorizon::ShortTerm;
    GoalState state = GoalState::Planned;
    std::string tag;
};

class LivingWorldGoalModel
{
public:
    static constexpr std::size_t MaximumGoals = 32;
    static constexpr std::size_t MaximumTagBytes = 48;
    static constexpr std::uint16_t MaximumPriorityBasisPoints = 10000;

    bool AddGoal(LivingWorldGoal goal);
    bool UpdateGoalState(
        std::uint64_t actorId,
        std::uint64_t goalId,
        GoalState newState,
        std::uint64_t simulationMinute);

    std::optional<LivingWorldGoal> FindGoal(std::uint64_t actorId, std::uint64_t goalId) const;
    std::vector<LivingWorldGoal> const& Goals() const;
    void Reset();

private:
    static bool IsSafeTag(std::string const& tag);
    static bool IsValidHorizon(GoalHorizon horizon);
    static bool IsValidState(GoalState state);
    static bool IsTerminal(GoalState state);
    static bool IsValidTransition(GoalState from, GoalState to);

    std::vector<LivingWorldGoal> goals_;
};
}

#endif
