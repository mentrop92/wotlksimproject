#include "LivingWorldPopulationPlan.h"

namespace LivingWorld
{
    PopulationExecutionPlan PopulationPlanBuilder::Build(
        PopulationDecision const& decision,
        std::vector<PopulationCandidate> const& candidates,
        bool emergencyDrain)
    {
        PopulationExecutionPlan plan;
        plan.decision = decision;
        plan.emergencyDrain = emergencyDrain;

        if (decision.requestedCount == 0 || decision.action == PopulationAction::None)
            return plan;

        if (decision.action == PopulationAction::Login)
        {
            plan.selectedCandidates = PopulationCandidateSelector::RankLoginCandidates(
                candidates,
                decision.requestedCount);
        }
        else if (decision.action == PopulationAction::Logout)
        {
            plan.selectedCandidates = PopulationCandidateSelector::RankLogoutCandidates(
                candidates,
                decision.requestedCount,
                emergencyDrain);
        }

        return plan;
    }
}
