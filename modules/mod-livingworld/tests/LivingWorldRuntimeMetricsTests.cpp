#include "LivingWorldRuntimeMetrics.h"

#include <cassert>
#include <iostream>

using namespace LivingWorld;

namespace
{
    RuntimeObservationSnapshot MakeSnapshot(
        std::uint32_t requested,
        std::uint32_t selected,
        std::uint32_t updateTime,
        std::uint32_t databaseLatency,
        bool emergency,
        bool constrained,
        bool healthy)
    {
        RuntimeObservationSnapshot snapshot;
        snapshot.bounded.requestedOperations = requested;
        snapshot.bounded.selectedCandidates = selected;
        snapshot.bounded.averageUpdateTimeMs = updateTime;
        snapshot.bounded.databaseLatencyMs = databaseLatency;
        snapshot.bounded.emergencyDrain = emergency;
        snapshot.unmetOperations = requested - selected;
        snapshot.totalBlockedCandidates = snapshot.unmetOperations * 2;
        snapshot.targetConstrained = constrained;
        snapshot.candidateShortfall = snapshot.unmetOperations > 0;
        snapshot.healthy = healthy;
        return snapshot;
    }
}

int main()
{
    RuntimeMetricWindow window(3);
    assert(window.Capacity() == 3);
    assert(window.Size() == 0);

    window.Add(MakeSnapshot(5, 5, 10, 4, false, false, true));
    window.Add(MakeSnapshot(8, 6, 20, 8, false, true, true));
    window.Add(MakeSnapshot(4, 1, 30, 12, true, true, false));

    RuntimeMetricSummary summary = window.Summarize();
    assert(summary.sampleCount == 3);
    assert(summary.unhealthySamples == 1);
    assert(summary.emergencyDrainSamples == 1);
    assert(summary.constrainedTargetSamples == 2);
    assert(summary.candidateShortfallSamples == 2);
    assert(summary.requestedOperations == 17);
    assert(summary.selectedCandidates == 12);
    assert(summary.unmetOperations == 5);
    assert(summary.blockedCandidates == 10);
    assert(summary.averageUpdateTimeMs == 20);
    assert(summary.averageDatabaseLatencyMs == 8);
    assert(summary.peakUpdateTimeMs == 30);
    assert(summary.peakDatabaseLatencyMs == 12);

    // The rolling window evicts the oldest sample deterministically.
    window.Add(MakeSnapshot(2, 2, 40, 16, false, false, true));
    summary = window.Summarize();
    assert(summary.sampleCount == 3);
    assert(summary.requestedOperations == 14);
    assert(summary.selectedCandidates == 9);
    assert(summary.averageUpdateTimeMs == 30);
    assert(summary.averageDatabaseLatencyMs == 12);
    assert(summary.peakUpdateTimeMs == 40);

    window.Reset();
    summary = window.Summarize();
    assert(window.Size() == 0);
    assert(summary.sampleCount == 0);
    assert(summary.requestedOperations == 0);

    RuntimeMetricWindow boundedLow(0);
    RuntimeMetricWindow boundedHigh(1000);
    assert(boundedLow.Capacity() == 1);
    assert(boundedHigh.Capacity() == RuntimeMetricLimits::MaximumWindowSamples);

    std::cout << "LivingWorld runtime metric window tests passed.\n";
    return 0;
}
