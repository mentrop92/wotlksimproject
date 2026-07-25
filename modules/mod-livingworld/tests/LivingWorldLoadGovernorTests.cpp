#include "LivingWorldLoadGovernor.h"

#include <cassert>
#include <iostream>

using namespace LivingWorld;

int main()
{
    ServerLoadPolicy policy;

    ServerLoadDecision healthy = LoadGovernor::Evaluate(400, ServerLoadSnapshot{}, policy);
    assert(healthy.governedTarget == 400);
    assert(healthy.targetPercent == 100);
    assert(!healthy.constrained);
    assert(!healthy.emergencyDrain);

    ServerLoadSnapshot warningSnapshot;
    warningSnapshot.averageWorldUpdateMs = policy.warningWorldUpdateMs;
    ServerLoadDecision warning = LoadGovernor::Evaluate(400, warningSnapshot, policy);
    assert(warning.governedTarget == 240);
    assert(warning.targetPercent == 60);
    assert(warning.constrained);
    assert(!warning.emergencyDrain);

    ServerLoadSnapshot databaseCritical;
    databaseCritical.databaseLatencyMs = policy.criticalDatabaseLatencyMs;
    ServerLoadDecision critical = LoadGovernor::Evaluate(400, databaseCritical, policy);
    assert(critical.governedTarget == 100);
    assert(critical.targetPercent == 25);
    assert(critical.constrained);
    assert(critical.emergencyDrain);

    ServerLoadSnapshot peakCritical;
    peakCritical.peakWorldUpdateMs = policy.criticalWorldUpdateMs * 2U;
    critical = LoadGovernor::Evaluate(3, peakCritical, policy);
    assert(critical.governedTarget == 0);
    assert(critical.emergencyDrain);

    policy.warningTargetPercent = 0;
    warning = LoadGovernor::Evaluate(100, warningSnapshot, policy);
    assert(warning.targetPercent == 1);
    assert(warning.governedTarget == 1);

    policy.criticalTargetPercent = 150;
    critical = LoadGovernor::Evaluate(100, databaseCritical, policy);
    assert(critical.targetPercent == 100);
    assert(critical.governedTarget == 100);
    assert(!critical.constrained);
    assert(critical.emergencyDrain);

    std::cout << "LivingWorld load governor tests passed.\n";
    return 0;
}
