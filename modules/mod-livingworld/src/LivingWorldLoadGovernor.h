#ifndef MOD_LIVINGWORLD_LOAD_GOVERNOR_H
#define MOD_LIVINGWORLD_LOAD_GOVERNOR_H

#include <cstdint>

namespace LivingWorld
{
    struct ServerLoadSnapshot
    {
        std::uint32_t averageWorldUpdateMs = 0;
        std::uint32_t peakWorldUpdateMs = 0;
        std::uint32_t databaseLatencyMs = 0;
        std::uint32_t queuedOperations = 0;
    };

    struct ServerLoadPolicy
    {
        std::uint32_t warningWorldUpdateMs = 75;
        std::uint32_t criticalWorldUpdateMs = 150;
        std::uint32_t warningDatabaseLatencyMs = 100;
        std::uint32_t criticalDatabaseLatencyMs = 300;
        std::uint32_t warningQueuedOperations = 50;
        std::uint32_t criticalQueuedOperations = 200;
        std::uint8_t warningTargetPercent = 60;
        std::uint8_t criticalTargetPercent = 25;
    };

    struct ServerLoadDecision
    {
        std::uint32_t requestedTarget = 0;
        std::uint32_t governedTarget = 0;
        std::uint8_t targetPercent = 100;
        bool constrained = false;
        bool emergencyDrain = false;
    };

    class LoadGovernor
    {
    public:
        static ServerLoadDecision Evaluate(
            std::uint32_t requestedTarget,
            ServerLoadSnapshot const& snapshot,
            ServerLoadPolicy const& policy = ServerLoadPolicy{});
    };
}

#endif
