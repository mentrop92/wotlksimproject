#ifndef MOD_LIVINGWORLD_GOAL_SERIALIZATION_H
#define MOD_LIVINGWORLD_GOAL_SERIALIZATION_H

#include "LivingWorldGoalModel.h"

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace LivingWorld
{
class LivingWorldGoalSerialization
{
public:
    static constexpr std::size_t MaximumPayloadBytes = 4096;
    static constexpr std::size_t MaximumSerializedGoals = LivingWorldGoalModel::MaximumGoals;

    static std::optional<std::string> Serialize(std::vector<LivingWorldGoal> const& goals);
    static std::optional<std::vector<LivingWorldGoal>> Deserialize(std::string const& payload);

private:
    static bool IsSafeTag(std::string const& tag);
};
}

#endif
