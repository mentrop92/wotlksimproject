#ifndef MOD_LIVINGWORLD_SIMULATION_OBSERVATION_HISTORY_H
#define MOD_LIVINGWORLD_SIMULATION_OBSERVATION_HISTORY_H

#include "LivingWorldSimulationRunObservations.h"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace LivingWorld
{
    namespace SimulationObservationHistoryLimits
    {
        inline constexpr std::size_t MaximumObservations = 128;
    }

    class SimulationObservationHistory
    {
    public:
        explicit SimulationObservationHistory(
            std::size_t capacity = SimulationObservationHistoryLimits::MaximumObservations);

        bool Append(SimulationRunObservation observation);
        void Reset();

        std::vector<SimulationRunObservation> const& Observations() const;
        std::optional<SimulationRunObservation> Latest() const;
        std::optional<SimulationRunObservation> FindLatestByCheckpointCorrelationId(
            std::string const& checkpointCorrelationId) const;
        std::optional<SimulationRunObservation> FindAtOrBefore(std::uint64_t simulationMinute) const;

        std::size_t Capacity() const;
        bool Clamped() const;

    private:
        std::size_t _capacity;
        bool _clamped = false;
        std::vector<SimulationRunObservation> _observations;
    };
}

#endif
