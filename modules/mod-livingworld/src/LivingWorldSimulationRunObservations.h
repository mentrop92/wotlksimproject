#ifndef MOD_LIVINGWORLD_SIMULATION_RUN_OBSERVATIONS_H
#define MOD_LIVINGWORLD_SIMULATION_RUN_OBSERVATIONS_H

#include "LivingWorldSimulationRunEvents.h"

#include <cstdint>
#include <string>

namespace LivingWorld
{
    namespace SimulationRunObservationLimits
    {
        inline constexpr std::size_t MaximumCheckpointCorrelationLength = 64;
    }

    struct SimulationRunObservation
    {
        std::uint64_t simulationMinute = 0;
        std::uint32_t progressBasisPoints = 0;
        std::uint32_t retainedEventCount = 0;
        std::uint32_t progressEventCount = 0;
        SimulationRunEventType latestEventType = SimulationRunEventType::Created;
        SimulationStopReason stopReason = SimulationStopReason::None;
        bool terminal = false;
        bool clamped = false;
        std::string checkpointCorrelationId;
    };

    class SimulationRunObservationBuilder
    {
    public:
        static SimulationRunObservation Build(
            SimulationRunEventHistory const& history,
            std::string checkpointCorrelationId = {});

    private:
        static std::string SanitizeCorrelationId(std::string value, bool& clamped);
        static bool IsTerminal(SimulationRunEventType type);
    };
}

#endif
