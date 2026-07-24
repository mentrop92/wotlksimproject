#include "LivingWorldPopulationDiagnostics.h"

#include <cassert>
#include <iostream>
#include <vector>

using namespace LivingWorld;

int main()
{
    std::vector<PopulationCandidate> candidates = {
        { 1, false, true, false, false, false, false, false, false, false, 0, 0, 0 },
        { 2, false, false, false, false, false, false, false, false, true, 30, 0, 0 },
        { 3, true, false, true, false, false, false, false, false, false, 0, 45, 80 },
        { 4, true, false, true, true, false, false, false, false, false, 0, 120, 100 },
        { 5, true, false, false, false, true, true, true, true, false, 0, 0, 20 },
        { 6, true, false, false, false, false, false, false, false, false, 0, 0, 10 }
    };

    PopulationDiagnostics const diagnostics = PopulationDiagnosticsBuilder::Analyze(candidates);

    assert(diagnostics.totalCandidates == 6);
    assert(diagnostics.offlineCandidates == 2);
    assert(diagnostics.onlineCandidates == 4);
    assert(diagnostics.loginReady == 1);
    assert(diagnostics.normalLogoutReady == 1);
    assert(diagnostics.emergencyOnly == 2);
    assert(diagnostics.blockedByCombat == 1);
    assert(diagnostics.blockedByInstance == 1);
    assert(diagnostics.blockedByLeadership == 1);
    assert(diagnostics.blockedByHumanInteraction == 1);
    assert(diagnostics.blockedByProtectedActivity == 0);

    std::cout << "LivingWorld population diagnostics tests passed.\n";
    return 0;
}
