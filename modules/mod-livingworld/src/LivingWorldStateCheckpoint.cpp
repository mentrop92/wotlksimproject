#include "LivingWorldStateCheckpoint.h"

#include <algorithm>

namespace LivingWorld
{
    StateCheckpoint StateCheckpointBuilder::Build(StateCheckpointInput const& input)
    {
        StateCheckpoint checkpoint;
        checkpoint.schemaVersion = input.schemaVersion;
        checkpoint.simulationMinute = input.simulationMinute;
        checkpoint.onlineAI = std::min(input.onlineAI, StateCheckpointLimits::MaximumPopulation);
        checkpoint.effectiveTarget = std::min(input.effectiveTarget, StateCheckpointLimits::MaximumPopulation);
        checkpoint.queuedOperations = std::min(input.queuedOperations, StateCheckpointLimits::MaximumPopulation);
        checkpoint.emergencyDrain = input.emergencyDrain;
        checkpoint.label = input.label.substr(0, StateCheckpointLimits::MaximumLabelLength);
        checkpoint.inputClamped = checkpoint.onlineAI != input.onlineAI ||
            checkpoint.effectiveTarget != input.effectiveTarget ||
            checkpoint.queuedOperations != input.queuedOperations ||
            checkpoint.label.size() != input.label.size();
        return checkpoint;
    }

    bool StateCheckpointBuilder::IsCompatible(StateCheckpoint const& checkpoint)
    {
        return checkpoint.schemaVersion == StateCheckpointLimits::CurrentSchemaVersion &&
            checkpoint.onlineAI <= StateCheckpointLimits::MaximumPopulation &&
            checkpoint.effectiveTarget <= StateCheckpointLimits::MaximumPopulation &&
            checkpoint.queuedOperations <= StateCheckpointLimits::MaximumPopulation &&
            checkpoint.label.size() <= StateCheckpointLimits::MaximumLabelLength;
    }
}
