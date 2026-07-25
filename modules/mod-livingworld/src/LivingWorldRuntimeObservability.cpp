#include "LivingWorldRuntimeObservability.h"

#include <algorithm>
#include <limits>

namespace LivingWorld
{
    namespace
    {
        std::uint32_t SaturatingAdd(std::uint32_t left, std::uint32_t right)
        {
            if (right > std::numeric_limits<std::uint32_t>::max() - left)
                return std::numeric_limits<std::uint32_t>::max();
            return left + right;
        }
    }

    RuntimeObservationSnapshot RuntimeObservability::Build(RuntimeObservationInput const& input)
    {
        RuntimeObservationSnapshot snapshot;
        snapshot.bounded = input;
        snapshot.bounded.blockedByCombat = std::min(input.blockedByCombat, ObservabilityLimits::MaximumBlockedCount);
        snapshot.bounded.blockedByInstance = std::min(input.blockedByInstance, ObservabilityLimits::MaximumBlockedCount);
        snapshot.bounded.blockedByHumanInteraction = std::min(input.blockedByHumanInteraction, ObservabilityLimits::MaximumBlockedCount);
        snapshot.bounded.blockedByProtectedActivity = std::min(input.blockedByProtectedActivity, ObservabilityLimits::MaximumBlockedCount);
        snapshot.bounded.averageUpdateTimeMs = std::min(input.averageUpdateTimeMs, ObservabilityLimits::MaximumUpdateTimeMs);
        snapshot.bounded.databaseLatencyMs = std::min(input.databaseLatencyMs, ObservabilityLimits::MaximumLatencyMs);
        snapshot.bounded.selectedCandidates = std::min(input.selectedCandidates, input.requestedOperations);

        snapshot.unmetOperations = snapshot.bounded.requestedOperations - snapshot.bounded.selectedCandidates;
        snapshot.totalBlockedCandidates = SaturatingAdd(
            SaturatingAdd(snapshot.bounded.blockedByCombat, snapshot.bounded.blockedByInstance),
            SaturatingAdd(snapshot.bounded.blockedByHumanInteraction, snapshot.bounded.blockedByProtectedActivity));
        snapshot.targetConstrained = snapshot.bounded.effectiveTarget < snapshot.bounded.configuredTarget ||
            snapshot.bounded.effectiveTarget > snapshot.bounded.effectiveAICeiling;
        snapshot.candidateShortfall = snapshot.unmetOperations > 0;
        snapshot.healthy = !snapshot.bounded.emergencyDrain &&
            snapshot.bounded.averageUpdateTimeMs < ObservabilityLimits::MaximumUpdateTimeMs &&
            snapshot.bounded.databaseLatencyMs < ObservabilityLimits::MaximumLatencyMs;
        return snapshot;
    }
}
