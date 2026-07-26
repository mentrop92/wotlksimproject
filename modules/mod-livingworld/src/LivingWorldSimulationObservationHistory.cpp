#include "LivingWorldSimulationObservationHistory.h"

#include <algorithm>

namespace LivingWorld
{
    SimulationObservationHistory::SimulationObservationHistory(std::size_t capacity)
        : _capacity(std::clamp<std::size_t>(
            capacity,
            1,
            SimulationObservationHistoryLimits::MaximumObservations))
    {
        _clamped = capacity == 0 || capacity > SimulationObservationHistoryLimits::MaximumObservations;
        _observations.reserve(_capacity);
    }

    bool SimulationObservationHistory::Append(SimulationRunObservation observation)
    {
        if (!_observations.empty() && observation.simulationMinute < _observations.back().simulationMinute)
            return false;

        if (_observations.size() == _capacity)
            _observations.erase(_observations.begin());

        _clamped = _clamped || observation.clamped;
        _observations.push_back(std::move(observation));
        return true;
    }

    void SimulationObservationHistory::Reset()
    {
        _observations.clear();
        _clamped = false;
    }

    std::vector<SimulationRunObservation> const& SimulationObservationHistory::Observations() const
    {
        return _observations;
    }

    std::optional<SimulationRunObservation> SimulationObservationHistory::Latest() const
    {
        if (_observations.empty())
            return std::nullopt;
        return _observations.back();
    }

    std::optional<SimulationRunObservation> SimulationObservationHistory::FindLatestByCheckpointCorrelationId(
        std::string const& checkpointCorrelationId) const
    {
        if (checkpointCorrelationId.empty())
            return std::nullopt;

        for (auto itr = _observations.rbegin(); itr != _observations.rend(); ++itr)
        {
            if (itr->checkpointCorrelationId == checkpointCorrelationId)
                return *itr;
        }
        return std::nullopt;
    }

    std::optional<SimulationRunObservation> SimulationObservationHistory::FindAtOrBefore(
        std::uint64_t simulationMinute) const
    {
        for (auto itr = _observations.rbegin(); itr != _observations.rend(); ++itr)
        {
            if (itr->simulationMinute <= simulationMinute)
                return *itr;
        }
        return std::nullopt;
    }

    std::size_t SimulationObservationHistory::Capacity() const
    {
        return _capacity;
    }

    bool SimulationObservationHistory::Clamped() const
    {
        return _clamped;
    }
}
