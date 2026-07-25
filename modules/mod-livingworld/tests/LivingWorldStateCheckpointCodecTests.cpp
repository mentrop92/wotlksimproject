#include "LivingWorldStateCheckpointCodec.h"

#include <cassert>
#include <iostream>
#include <string>

using namespace LivingWorld;

int main()
{
    StateCheckpointInput input;
    input.simulationMinute = 987654;
    input.onlineAI = 321;
    input.effectiveTarget = 400;
    input.queuedOperations = 7;
    input.emergencyDrain = true;
    input.label = "hourly|checkpoint\nvalidated";

    StateCheckpoint checkpoint = StateCheckpointBuilder::Build(input);
    std::optional<std::string> encoded = StateCheckpointCodec::Encode(checkpoint);
    assert(encoded);
    assert(encoded->size() <= StateCheckpointCodecLimits::MaximumEncodedSize);

    std::optional<StateCheckpoint> decoded = StateCheckpointCodec::Decode(*encoded);
    assert(decoded);
    assert(decoded->schemaVersion == checkpoint.schemaVersion);
    assert(decoded->simulationMinute == checkpoint.simulationMinute);
    assert(decoded->onlineAI == checkpoint.onlineAI);
    assert(decoded->effectiveTarget == checkpoint.effectiveTarget);
    assert(decoded->queuedOperations == checkpoint.queuedOperations);
    assert(decoded->emergencyDrain == checkpoint.emergencyDrain);
    assert(decoded->inputClamped == checkpoint.inputClamped);
    assert(decoded->label == checkpoint.label);

    assert(!StateCheckpointCodec::Decode(""));
    assert(!StateCheckpointCodec::Decode("LWCP1|1|0|0|0|0|0|0|0|EXTRA"));
    assert(!StateCheckpointCodec::Decode("LWCP1|1|not-a-number|0|0|0|0|0|"));
    assert(!StateCheckpointCodec::Decode("LWCP1|2|0|0|0|0|0|0|"));
    assert(!StateCheckpointCodec::Decode("LWCP1|1|0|10001|0|0|0|0|"));
    assert(!StateCheckpointCodec::Decode("LWCP1|1|0|0|0|0|2|0|"));
    assert(!StateCheckpointCodec::Decode("LWCP1|1|0|0|0|0|0|0|A"));
    assert(!StateCheckpointCodec::Decode(std::string(StateCheckpointCodecLimits::MaximumEncodedSize + 1, 'x')));

    checkpoint.schemaVersion = StateCheckpointLimits::CurrentSchemaVersion + 1;
    assert(!StateCheckpointCodec::Encode(checkpoint));

    std::cout << "LivingWorld state checkpoint codec tests passed.\n";
    return 0;
}
