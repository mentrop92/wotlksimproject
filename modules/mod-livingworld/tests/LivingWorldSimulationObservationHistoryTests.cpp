#include "LivingWorldSimulationObservationHistory.h"

#include <cassert>
#include <iostream>

using namespace LivingWorld;

namespace
{
    SimulationRunObservation Observation(
        std::uint64_t minute,
        std::string correlation = {},
        bool clamped = false)
    {
        SimulationRunObservation observation;
        observation.simulationMinute = minute;
        observation.checkpointCorrelationId = std::move(correlation);
        observation.clamped = clamped;
        return observation;
    }
}

int main()
{
    SimulationObservationHistory history(3);
    assert(history.Capacity() == 3);
    assert(!history.Clamped());
    assert(!history.Latest().has_value());

    assert(history.Append(Observation(10, "checkpoint-a")));
    assert(history.Append(Observation(20, "checkpoint-b")));
    assert(history.Append(Observation(30, "checkpoint-a", true)));
    assert(history.Observations().size() == 3);
    assert(history.Clamped());

    auto latest = history.Latest();
    assert(latest.has_value());
    assert(latest->simulationMinute == 30);

    auto correlated = history.FindLatestByCheckpointCorrelationId("checkpoint-a");
    assert(correlated.has_value());
    assert(correlated->simulationMinute == 30);
    assert(!history.FindLatestByCheckpointCorrelationId("").has_value());
    assert(!history.FindLatestByCheckpointCorrelationId("missing").has_value());

    auto atOrBefore = history.FindAtOrBefore(25);
    assert(atOrBefore.has_value());
    assert(atOrBefore->simulationMinute == 20);
    assert(!history.FindAtOrBefore(5).has_value());

    assert(history.Append(Observation(40, "checkpoint-c")));
    assert(history.Observations().size() == 3);
    assert(history.Observations().front().simulationMinute == 20);

    assert(!history.Append(Observation(39, "out-of-order")));
    assert(history.Observations().back().simulationMinute == 40);

    history.Reset();
    assert(history.Observations().empty());
    assert(!history.Clamped());

    SimulationObservationHistory zeroCapacity(0);
    assert(zeroCapacity.Capacity() == 1);
    assert(zeroCapacity.Clamped());

    SimulationObservationHistory oversized(
        SimulationObservationHistoryLimits::MaximumObservations + 50);
    assert(oversized.Capacity() == SimulationObservationHistoryLimits::MaximumObservations);
    assert(oversized.Clamped());

    std::cout << "LivingWorld simulation observation-history tests passed.\n";
    return 0;
}
