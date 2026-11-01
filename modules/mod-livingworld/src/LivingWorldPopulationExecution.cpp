#include "LivingWorldPopulationExecution.h"

#include <algorithm>

namespace LivingWorld
{
    PopulationExecutionPlan PopulationExecutionPlanner::Plan(
        PopulationDecision const& decision,
        std::uint32_t onlineRandomBots,
        std::uint32_t currentBotCountTarget,
        PlayerbotPopulationLimits const& limits,
        std::vector<PopulationCandidate> const& logoutCandidates)
    {
        PopulationExecutionPlan plan;

        std::uint32_t desired = onlineRandomBots;
        if (decision.action == PopulationAction::Login)
            desired = onlineRandomBots + decision.requestedCount;
        else if (decision.action == PopulationAction::Logout)
        {
            std::vector<RankedCandidate> const ranked = PopulationCandidateSelector::RankLogoutCandidates(
                logoutCandidates,
                decision.requestedCount,
                false);

            for (RankedCandidate const& candidate : ranked)
                plan.logoutGuids.push_back(candidate.characterGuid);

            // Only lower the target by bots that will actually leave; otherwise
            // the manager would log in replacements for protected bots.
            std::uint32_t const leaving = static_cast<std::uint32_t>(plan.logoutGuids.size());
            desired = onlineRandomBots > leaving ? onlineRandomBots - leaving : 0;
        }

        std::uint32_t const upper = std::max(limits.minimumRandomBots, limits.maximumRandomBots);
        plan.botCountTarget = std::clamp(desired, limits.minimumRandomBots, upper);
        plan.clampedToPlayerbotLimits = plan.botCountTarget != desired;
        plan.writeBotCountTarget = plan.botCountTarget != currentBotCountTarget;
        return plan;
    }
}
