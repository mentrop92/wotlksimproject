#ifndef MOD_LIVINGWORLD_STATE_CHECKPOINT_H
#define MOD_LIVINGWORLD_STATE_CHECKPOINT_H

#include <cstdint>
#include <string>

namespace LivingWorld
{
    namespace StateCheckpointLimits
    {
        inline constexpr std::uint32_t CurrentSchemaVersion = 1;
        inline constexpr std::uint32_t MaximumPopulation = 10000;
        inline constexpr std::uint32_t MaximumElapsedMinutes = 525600;
        inline constexpr std::size_t MaximumLabelLength = 64;
    }

    struct StateCheckpointInput
    {
        std::uint32_t schemaVersion = StateCheckpointLimits::CurrentSchemaVersion;
        std::uint64_t simulationMinute = 0;
        std::uint32_t onlineAI = 0;
        std::uint32_t effectiveTarget = 0;
        std::uint32_t queuedOperations = 0;
        bool emergencyDrain = false;
        std::string label;
    };

    struct StateCheckpoint
    {
        std::uint32_t schemaVersion = StateCheckpointLimits::CurrentSchemaVersion;
        std::uint64_t simulationMinute = 0;
        std::uint32_t onlineAI = 0;
        std::uint32_t effectiveTarget = 0;
        std::uint32_t queuedOperations = 0;
        bool emergencyDrain = false;
        std::string label;
        bool inputClamped = false;
    };

    class StateCheckpointBuilder
    {
    public:
        static StateCheckpoint Build(StateCheckpointInput const& input);
        static bool IsCompatible(StateCheckpoint const& checkpoint);
    };
}

#endif
