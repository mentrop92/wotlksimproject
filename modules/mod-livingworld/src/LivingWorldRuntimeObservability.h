#ifndef MOD_LIVINGWORLD_RUNTIME_OBSERVABILITY_H
#define MOD_LIVINGWORLD_RUNTIME_OBSERVABILITY_H

#include <cstdint>

namespace LivingWorld
{
    namespace ObservabilityLimits
    {
        inline constexpr std::uint32_t MaximumBlockedCount = 10000;
        inline constexpr std::uint32_t MaximumLatencyMs = 60000;
        inline constexpr std::uint32_t MaximumUpdateTimeMs = 60000;
    }

    enum class PlannedPopulationAction : std::uint8_t
    {
        None = 0,
        Login,
        Logout
    };

    struct RuntimeObservationInput
    {
        std::uint32_t onlineAI = 0;
        std::uint32_t onlineHumans = 0;
        std::uint32_t configuredTarget = 0;
        std::uint32_t effectiveTarget = 0;
        std::uint32_t effectiveAICeiling = 0;
        PlannedPopulationAction plannedAction = PlannedPopulationAction::None;
        std::uint32_t requestedOperations = 0;
        std::uint32_t selectedCandidates = 0;
        std::uint32_t blockedByCombat = 0;
        std::uint32_t blockedByInstance = 0;
        std::uint32_t blockedByHumanInteraction = 0;
        std::uint32_t blockedByProtectedActivity = 0;
        std::uint32_t averageUpdateTimeMs = 0;
        std::uint32_t databaseLatencyMs = 0;
        bool emergencyDrain = false;
    };

    struct RuntimeObservationSnapshot
    {
        RuntimeObservationInput bounded;
        std::uint32_t unmetOperations = 0;
        std::uint32_t totalBlockedCandidates = 0;
        bool targetConstrained = false;
        bool candidateShortfall = false;
        bool healthy = true;
    };

    class RuntimeObservability
    {
    public:
        static RuntimeObservationSnapshot Build(RuntimeObservationInput const& input);
    };
}

#endif
