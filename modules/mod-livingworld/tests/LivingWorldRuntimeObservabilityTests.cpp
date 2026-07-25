#include "LivingWorldRuntimeObservability.h"

#include <cassert>
#include <cstdint>
#include <iostream>

using namespace LivingWorld;

int main()
{
    RuntimeObservationInput healthy;
    healthy.onlineAI = 90;
    healthy.onlineHumans = 10;
    healthy.configuredTarget = 100;
    healthy.effectiveTarget = 100;
    healthy.effectiveAICeiling = 200;
    healthy.plannedAction = PlannedPopulationAction::Login;
    healthy.requestedOperations = 5;
    healthy.selectedCandidates = 5;
    healthy.averageUpdateTimeMs = 25;
    healthy.databaseLatencyMs = 10;

    RuntimeObservationSnapshot snapshot = RuntimeObservability::Build(healthy);
    assert(snapshot.unmetOperations == 0);
    assert(snapshot.totalBlockedCandidates == 0);
    assert(!snapshot.targetConstrained);
    assert(!snapshot.candidateShortfall);
    assert(snapshot.healthy);

    RuntimeObservationInput constrained = healthy;
    constrained.effectiveTarget = 60;
    constrained.requestedOperations = 10;
    constrained.selectedCandidates = 3;
    constrained.blockedByCombat = 4;
    constrained.blockedByInstance = 2;
    constrained.blockedByHumanInteraction = 1;
    constrained.blockedByProtectedActivity = 5;
    snapshot = RuntimeObservability::Build(constrained);
    assert(snapshot.unmetOperations == 7);
    assert(snapshot.totalBlockedCandidates == 12);
    assert(snapshot.targetConstrained);
    assert(snapshot.candidateShortfall);
    assert(snapshot.healthy);

    RuntimeObservationInput hostile;
    hostile.requestedOperations = 2;
    hostile.selectedCandidates = 100;
    hostile.blockedByCombat = UINT32_MAX;
    hostile.blockedByInstance = UINT32_MAX;
    hostile.blockedByHumanInteraction = UINT32_MAX;
    hostile.blockedByProtectedActivity = UINT32_MAX;
    hostile.averageUpdateTimeMs = UINT32_MAX;
    hostile.databaseLatencyMs = UINT32_MAX;
    hostile.emergencyDrain = true;
    snapshot = RuntimeObservability::Build(hostile);
    assert(snapshot.bounded.selectedCandidates == 2);
    assert(snapshot.unmetOperations == 0);
    assert(snapshot.bounded.blockedByCombat == ObservabilityLimits::MaximumBlockedCount);
    assert(snapshot.totalBlockedCandidates == 40000);
    assert(snapshot.bounded.averageUpdateTimeMs == ObservabilityLimits::MaximumUpdateTimeMs);
    assert(snapshot.bounded.databaseLatencyMs == ObservabilityLimits::MaximumLatencyMs);
    assert(!snapshot.healthy);

    std::cout << "LivingWorld runtime observability tests passed.\n";
    return 0;
}
