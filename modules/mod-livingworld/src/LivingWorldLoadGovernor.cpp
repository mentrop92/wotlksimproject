#include "LivingWorldLoadGovernor.h"

#include <algorithm>

namespace LivingWorld
{
    namespace
    {
        std::uint8_t ClampPercent(std::uint8_t value)
        {
            return std::clamp<std::uint8_t>(value, 1, 100);
        }
    }

    ServerLoadDecision LoadGovernor::Evaluate(
        std::uint32_t requestedTarget,
        ServerLoadSnapshot const& snapshot,
        ServerLoadPolicy const& policy)
    {
        ServerLoadDecision decision;
        decision.requestedTarget = requestedTarget;

        bool const critical =
            snapshot.averageWorldUpdateMs >= policy.criticalWorldUpdateMs ||
            snapshot.peakWorldUpdateMs >= policy.criticalWorldUpdateMs * 2U ||
            snapshot.databaseLatencyMs >= policy.criticalDatabaseLatencyMs ||
            snapshot.queuedOperations >= policy.criticalQueuedOperations;

        bool const warning =
            snapshot.averageWorldUpdateMs >= policy.warningWorldUpdateMs ||
            snapshot.peakWorldUpdateMs >= policy.warningWorldUpdateMs * 2U ||
            snapshot.databaseLatencyMs >= policy.warningDatabaseLatencyMs ||
            snapshot.queuedOperations >= policy.warningQueuedOperations;

        if (critical)
        {
            decision.targetPercent = ClampPercent(policy.criticalTargetPercent);
            decision.emergencyDrain = true;
        }
        else if (warning)
        {
            decision.targetPercent = ClampPercent(policy.warningTargetPercent);
        }

        decision.governedTarget = static_cast<std::uint32_t>(
            (static_cast<std::uint64_t>(requestedTarget) * decision.targetPercent) / 100ULL);
        decision.constrained = decision.governedTarget < requestedTarget;
        return decision;
    }
}
