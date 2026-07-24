#include "LivingWorldPopulationDiagnostics.h"

namespace LivingWorld
{
    PopulationDiagnostics PopulationDiagnosticsBuilder::Analyze(
        std::vector<PopulationCandidate> const& candidates)
    {
        PopulationDiagnostics diagnostics;
        diagnostics.totalCandidates = static_cast<std::uint32_t>(candidates.size());

        for (PopulationCandidate const& candidate : candidates)
        {
            if (!candidate.online)
            {
                ++diagnostics.offlineCandidates;
                if (candidate.scheduleDue && !candidate.protectedActivity)
                    ++diagnostics.loginReady;
                continue;
            }

            ++diagnostics.onlineCandidates;

            bool blocked = false;
            if (candidate.inCombat)
            {
                ++diagnostics.blockedByCombat;
                blocked = true;
            }
            if (candidate.inInstance)
            {
                ++diagnostics.blockedByInstance;
                blocked = true;
            }
            if (candidate.groupLeader)
            {
                ++diagnostics.blockedByLeadership;
                blocked = true;
            }
            if (candidate.interactingWithHuman)
            {
                ++diagnostics.blockedByHumanInteraction;
                blocked = true;
            }
            if (candidate.protectedActivity)
            {
                ++diagnostics.blockedByProtectedActivity;
                blocked = true;
            }

            if (blocked)
                ++diagnostics.emergencyOnly;
            else if (candidate.sessionComplete || candidate.logoutReadiness >= 50)
                ++diagnostics.normalLogoutReady;
        }

        return diagnostics;
    }
}
