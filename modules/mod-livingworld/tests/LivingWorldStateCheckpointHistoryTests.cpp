#include "LivingWorldStateCheckpointHistory.h"

#include <cassert>
#include <iostream>

using namespace LivingWorld;

namespace
{
    StateCheckpoint MakeCheckpoint(std::uint64_t minute, std::uint32_t online)
    {
        StateCheckpointInput input;
        input.simulationMinute = minute;
        input.onlineAI = online;
        input.effectiveTarget = online;
        input.label = "history";
        return StateCheckpointBuilder::Build(input);
    }
}

int main()
{
    StateCheckpointHistory history(3);
    assert(history.Capacity() == 3);
    assert(!history.Latest());

    assert(history.Push(MakeCheckpoint(10, 100)));
    assert(history.Push(MakeCheckpoint(20, 200)));
    assert(history.Push(MakeCheckpoint(30, 300)));
    assert(history.Size() == 3);
    assert(history.Latest()->simulationMinute == 30);
    assert(history.FindAtOrBefore(25)->simulationMinute == 20);
    assert(!history.FindAtOrBefore(5));

    // Capacity eviction is deterministic and removes only the oldest entry.
    assert(history.Push(MakeCheckpoint(40, 400)));
    assert(history.Size() == 3);
    assert(!history.FindAtOrBefore(15));
    assert(history.FindAtOrBefore(40)->simulationMinute == 40);

    // Rewinding simulation time is rejected; equal-minute replacement snapshots
    // are permitted so callers can record a corrected observation at one tick.
    assert(!history.Push(MakeCheckpoint(39, 390)));
    assert(history.Push(MakeCheckpoint(40, 401)));
    assert(history.Latest()->onlineAI == 401);

    StateCheckpoint incompatible = MakeCheckpoint(50, 500);
    incompatible.schemaVersion += 1;
    assert(!history.Push(incompatible));

    StateCheckpointHistory clampedCapacity(1000);
    assert(clampedCapacity.Capacity() == StateCheckpointHistoryLimits::MaximumEntries);

    history.Clear();
    assert(history.Size() == 0);
    assert(!history.Latest());

    std::cout << "LivingWorld state checkpoint history tests passed.\n";
    return 0;
}
