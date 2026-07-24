#include "LivingWorldPopulationPlan.h"

#include <cassert>
#include <iostream>
#include <vector>

using namespace LivingWorld;

int main()
{
    std::vector<PopulationCandidate> candidates = {
        { 1, false, true, false, false, false, false, false, false, false, 0, 0 },
        { 2, false, false, false, false, false, false, false, false, false, 30, 0 },
        { 3, true, false, true, false, false, false, false, false, false, 0, 20 },
        { 4, true, false, true, true, false, false, false, false, false, 0, 60 },
        { 5, true, false, true, false, false, false, false, true, false, 0, 90 }
    };

    PopulationDecision loginDecision;
    loginDecision.action = PopulationAction::Login;
    loginDecision.requestedCount = 2;

    PopulationExecutionPlan loginPlan = PopulationPlanBuilder::Build(loginDecision, candidates);
    assert(loginPlan.selectedCandidates.size() == 2);
    assert(loginPlan.selectedCandidates[0].characterGuid == 1);
    assert(loginPlan.selectedCandidates[1].characterGuid == 2);
    assert(!loginPlan.emergencyDrain);

    PopulationDecision logoutDecision;
    logoutDecision.action = PopulationAction::Logout;
    logoutDecision.requestedCount = 3;

    PopulationExecutionPlan normalLogoutPlan = PopulationPlanBuilder::Build(logoutDecision, candidates, false);
    assert(normalLogoutPlan.selectedCandidates.size() == 1);
    assert(normalLogoutPlan.selectedCandidates[0].characterGuid == 3);

    PopulationExecutionPlan emergencyPlan = PopulationPlanBuilder::Build(logoutDecision, candidates, true);
    assert(emergencyPlan.selectedCandidates.size() == 3);
    assert(emergencyPlan.selectedCandidates[0].characterGuid == 3);
    assert(emergencyPlan.emergencyDrain);

    PopulationDecision noneDecision;
    PopulationExecutionPlan nonePlan = PopulationPlanBuilder::Build(noneDecision, candidates);
    assert(nonePlan.selectedCandidates.empty());

    std::cout << "LivingWorld population plan tests passed.\n";
    return 0;
}
