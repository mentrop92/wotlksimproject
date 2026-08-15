#include "LivingWorldGoalSerialization.h"

#include <algorithm>
#include <charconv>
#include <cctype>
#include <cstdint>
#include <limits>
#include <sstream>
#include <string_view>

namespace LivingWorld
{
namespace
{
constexpr std::string_view VersionHeader = "LWGOAL1";

bool ParseUnsigned(std::string_view text, std::uint64_t& value)
{
    if (text.empty())
        return false;
    auto first = text.data();
    auto last = text.data() + text.size();
    auto result = std::from_chars(first, last, value);
    return result.ec == std::errc{} && result.ptr == last;
}

bool SplitRecord(std::string const& line, std::vector<std::string_view>& fields)
{
    fields.clear();
    std::size_t start = 0;
    while (start <= line.size())
    {
        std::size_t comma = line.find(',', start);
        if (comma == std::string::npos)
        {
            fields.emplace_back(line.data() + start, line.size() - start);
            break;
        }
        fields.emplace_back(line.data() + start, comma - start);
        start = comma + 1;
    }
    return fields.size() == 8;
}
}

bool LivingWorldGoalSerialization::IsSafeTag(std::string const& tag)
{
    if (tag.empty() || tag.size() > LivingWorldGoalModel::MaximumTagBytes)
        return false;
    return std::all_of(tag.begin(), tag.end(), [](unsigned char ch)
    {
        return std::isalnum(ch) || ch == '_' || ch == '-' || ch == '.';
    });
}

std::optional<std::string> LivingWorldGoalSerialization::Serialize(std::vector<LivingWorldGoal> const& goals)
{
    if (goals.size() > MaximumSerializedGoals)
        return std::nullopt;

    LivingWorldGoalModel validation;
    std::ostringstream out;
    out << VersionHeader << '\n';

    for (LivingWorldGoal const& goal : goals)
    {
        if (!validation.AddGoal(goal) || !IsSafeTag(goal.tag))
            return std::nullopt;

        out << goal.actorId << ','
            << goal.goalId << ','
            << goal.createdSimulationMinute << ','
            << goal.updatedSimulationMinute << ','
            << goal.priorityBasisPoints << ','
            << static_cast<unsigned int>(goal.horizon) << ','
            << static_cast<unsigned int>(goal.state) << ','
            << goal.tag << '\n';

        if (out.tellp() < 0 || static_cast<std::size_t>(out.tellp()) > MaximumPayloadBytes)
            return std::nullopt;
    }

    std::string payload = out.str();
    if (payload.size() > MaximumPayloadBytes)
        return std::nullopt;
    return payload;
}

std::optional<std::vector<LivingWorldGoal>> LivingWorldGoalSerialization::Deserialize(std::string const& payload)
{
    if (payload.empty() || payload.size() > MaximumPayloadBytes)
        return std::nullopt;

    std::istringstream input(payload);
    std::string line;
    if (!std::getline(input, line) || line != VersionHeader)
        return std::nullopt;

    LivingWorldGoalModel validation;
    std::vector<std::string_view> fields;
    std::size_t count = 0;

    while (std::getline(input, line))
    {
        if (line.empty())
            return std::nullopt;
        if (++count > MaximumSerializedGoals || !SplitRecord(line, fields))
            return std::nullopt;

        std::uint64_t actorId = 0;
        std::uint64_t goalId = 0;
        std::uint64_t created = 0;
        std::uint64_t updated = 0;
        std::uint64_t priority = 0;
        std::uint64_t horizon = 0;
        std::uint64_t state = 0;

        if (!ParseUnsigned(fields[0], actorId) || !ParseUnsigned(fields[1], goalId) ||
            !ParseUnsigned(fields[2], created) || !ParseUnsigned(fields[3], updated) ||
            !ParseUnsigned(fields[4], priority) || !ParseUnsigned(fields[5], horizon) ||
            !ParseUnsigned(fields[6], state))
            return std::nullopt;
        if (priority > LivingWorldGoalModel::MaximumPriorityBasisPoints || horizon > 2 || state > 3)
            return std::nullopt;

        LivingWorldGoal goal;
        goal.actorId = actorId;
        goal.goalId = goalId;
        goal.createdSimulationMinute = created;
        goal.updatedSimulationMinute = updated;
        goal.priorityBasisPoints = static_cast<std::uint16_t>(priority);
        goal.horizon = static_cast<GoalHorizon>(horizon);
        goal.state = static_cast<GoalState>(state);
        goal.tag.assign(fields[7].data(), fields[7].size());

        if (!IsSafeTag(goal.tag) || !validation.AddGoal(goal))
            return std::nullopt;
    }

    return validation.Goals();
}
}
