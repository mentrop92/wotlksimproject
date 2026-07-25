#include "LivingWorldStateCheckpoint.h"

#include <cassert>
#include <iostream>
#include <string>

using namespace LivingWorld;

int main()
{
    StateCheckpointInput normal;
    normal.simulationMinute = 1440;
    normal.onlineAI = 250;
    normal.effectiveTarget = 300;
    normal.queuedOperations = 4;
    normal.label = "hourly-runtime-checkpoint";

    StateCheckpoint checkpoint = StateCheckpointBuilder::Build(normal);
    assert(checkpoint.schemaVersion == StateCheckpointLimits::CurrentSchemaVersion);
    assert(checkpoint.simulationMinute == 1440);
    assert(checkpoint.onlineAI == 250);
    assert(checkpoint.effectiveTarget == 300);
    assert(checkpoint.queuedOperations == 4);
    assert(!checkpoint.inputClamped);
    assert(StateCheckpointBuilder::IsCompatible(checkpoint));

    StateCheckpointInput unsafe;
    unsafe.onlineAI = 50000;
    unsafe.effectiveTarget = 60000;
    unsafe.queuedOperations = 70000;
    unsafe.label = std::string(200, 'x');

    checkpoint = StateCheckpointBuilder::Build(unsafe);
    assert(checkpoint.onlineAI == StateCheckpointLimits::MaximumPopulation);
    assert(checkpoint.effectiveTarget == StateCheckpointLimits::MaximumPopulation);
    assert(checkpoint.queuedOperations == StateCheckpointLimits::MaximumPopulation);
    assert(checkpoint.label.size() == StateCheckpointLimits::MaximumLabelLength);
    assert(checkpoint.inputClamped);
    assert(StateCheckpointBuilder::IsCompatible(checkpoint));

    checkpoint.schemaVersion = StateCheckpointLimits::CurrentSchemaVersion + 1;
    assert(!StateCheckpointBuilder::IsCompatible(checkpoint));

    std::cout << "LivingWorld state checkpoint tests passed.\n";
    return 0;
}
