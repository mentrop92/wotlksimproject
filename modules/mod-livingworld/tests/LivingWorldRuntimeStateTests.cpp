#include "LivingWorldRuntimeState.h"

#include <cassert>
#include <iostream>

using namespace LivingWorld;

int main()
{
    RuntimeState& state = RuntimeState::Instance();
    state.Reset();
    assert(!state.HasSamples());
    assert(state.Summary().sampleCount == 0);

    RuntimeObservationInput healthy;
    healthy.onlineAI = 80;
    healthy.onlineHumans = 10;
    healthy.configuredTarget = 100;
    healthy.effectiveTarget = 100;
    healthy.effectiveAICeiling = 200;
    healthy.plannedAction = PlannedPopulationAction::Login;
    healthy.requestedOperations = 2;
    healthy.selectedCandidates = 2;
    healthy.averageUpdateTimeMs = 20;
    healthy.databaseLatencyMs = 10;

    RuntimeObservationSnapshot first = state.Record(healthy);
    assert(state.HasSamples());
    assert(first.healthy);
    assert(first.unmetOperations == 0);
    assert(state.Latest().bounded.onlineAI == 80);

    RuntimeObservationInput unhealthy = healthy;
    unhealthy.requestedOperations = 5;
    unhealthy.selectedCandidates = 1;
    unhealthy.blockedByCombat = 3;
    unhealthy.averageUpdateTimeMs = 60000;
    unhealthy.emergencyDrain = true;
    state.Record(unhealthy);

    RuntimeMetricSummary summary = state.Summary();
    assert(summary.sampleCount == 2);
    assert(summary.unhealthySamples == 1);
    assert(summary.emergencyDrainSamples == 1);
    assert(summary.requestedOperations == 7);
    assert(summary.selectedCandidates == 3);
    assert(summary.unmetOperations == 4);
    assert(summary.blockedCandidates == 3);

    state.Reset();
    assert(!state.HasSamples());
    assert(state.Summary().sampleCount == 0);

    std::cout << "LivingWorld runtime state tests passed.\n";
    return 0;
}
