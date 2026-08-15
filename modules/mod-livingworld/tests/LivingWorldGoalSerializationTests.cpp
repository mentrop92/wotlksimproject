#include "LivingWorldGoalSerialization.h"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using namespace LivingWorld;

namespace
{
LivingWorldGoal MakeGoal(
    std::uint64_t actorId,
    std::uint64_t goalId,
    std::uint64_t minute,
    std::uint16_t priority,
    GoalHorizon horizon,
    GoalState state,
    std::string tag)
{
    LivingWorldGoal goal;
    goal.actorId = actorId;
    goal.goalId = goalId;
    goal.createdSimulationMinute = minute;
    goal.updatedSimulationMinute = minute + 5;
    goal.priorityBasisPoints = priority;
    goal.horizon = horizon;
    goal.state = state;
    goal.tag = std::move(tag);
    return goal;
}
}

int main()
{
    std::vector<LivingWorldGoal> goals;
    goals.push_back(MakeGoal(1, 10, 100, 7000, GoalHorizon::ShortTerm, GoalState::Active, "level_to_30"));
    goals.push_back(MakeGoal(1, 11, 120, 5000, GoalHorizon::MediumTerm, GoalState::Planned, "learn_profession"));
    goals.push_back(MakeGoal(2, 20, 140, 9000, GoalHorizon::LongTerm, GoalState::Completed, "lead_guild"));

    auto encoded = LivingWorldGoalSerialization::Serialize(goals);
    assert(encoded.has_value());
    assert(encoded->size() <= LivingWorldGoalSerialization::MaximumPayloadBytes);

    auto encodedAgain = LivingWorldGoalSerialization::Serialize(goals);
    assert(encodedAgain.has_value());
    assert(*encoded == *encodedAgain);

    auto decoded = LivingWorldGoalSerialization::Deserialize(*encoded);
    assert(decoded.has_value());
    assert(decoded->size() == goals.size());
    for (std::size_t i = 0; i < goals.size(); ++i)
    {
        assert((*decoded)[i].actorId == goals[i].actorId);
        assert((*decoded)[i].goalId == goals[i].goalId);
        assert((*decoded)[i].createdSimulationMinute == goals[i].createdSimulationMinute);
        assert((*decoded)[i].updatedSimulationMinute == goals[i].updatedSimulationMinute);
        assert((*decoded)[i].priorityBasisPoints == goals[i].priorityBasisPoints);
        assert((*decoded)[i].horizon == goals[i].horizon);
        assert((*decoded)[i].state == goals[i].state);
        assert((*decoded)[i].tag == goals[i].tag);
    }

    auto emptyEncoded = LivingWorldGoalSerialization::Serialize({});
    assert(emptyEncoded.has_value());
    auto emptyDecoded = LivingWorldGoalSerialization::Deserialize(*emptyEncoded);
    assert(emptyDecoded.has_value() && emptyDecoded->empty());

    std::vector<LivingWorldGoal> tooMany;
    for (std::size_t i = 0; i <= LivingWorldGoalSerialization::MaximumSerializedGoals; ++i)
        tooMany.push_back(MakeGoal(9, 1000 + i, i, 1000, GoalHorizon::ShortTerm, GoalState::Planned, "bounded"));
    assert(!LivingWorldGoalSerialization::Serialize(tooMany).has_value());

    auto invalid = goals;
    invalid[0].tag = "unsafe tag";
    assert(!LivingWorldGoalSerialization::Serialize(invalid).has_value());

    invalid = goals;
    invalid.push_back(goals.front());
    assert(!LivingWorldGoalSerialization::Serialize(invalid).has_value());

    assert(!LivingWorldGoalSerialization::Deserialize("LWGOAL2\n").has_value());
    assert(!LivingWorldGoalSerialization::Deserialize("LWGOAL1\n1,2,3,4,100,0,0,unsafe tag\n").has_value());
    assert(!LivingWorldGoalSerialization::Deserialize("LWGOAL1\n1,2,3,4,10001,0,0,goal\n").has_value());
    assert(!LivingWorldGoalSerialization::Deserialize("LWGOAL1\n1,2,3,4,100,9,0,goal\n").has_value());
    assert(!LivingWorldGoalSerialization::Deserialize("LWGOAL1\n1,2,3,4,100,0,9,goal\n").has_value());
    assert(!LivingWorldGoalSerialization::Deserialize("LWGOAL1\n1,2,4,3,100,0,0,goal\n").has_value());
    assert(!LivingWorldGoalSerialization::Deserialize("LWGOAL1\n1,2,3,4,100,0,0,goal\n1,2,3,4,100,0,0,goal\n").has_value());

    std::string oversized(LivingWorldGoalSerialization::MaximumPayloadBytes + 1, 'x');
    assert(!LivingWorldGoalSerialization::Deserialize(oversized).has_value());

    std::cout << "LivingWorld bounded goal serialization tests passed.\n";
    return 0;
}
