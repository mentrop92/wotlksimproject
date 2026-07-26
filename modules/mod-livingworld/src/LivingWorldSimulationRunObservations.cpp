#include "LivingWorldSimulationRunObservations.h"

#include <algorithm>

namespace LivingWorld
{
    SimulationRunObservation SimulationRunObservationBuilder::Build(
        SimulationRunEventHistory const& history,
        std::string checkpointCorrelationId)
    {
        SimulationRunObservation observation;
        observation.checkpointCorrelationId = SanitizeCorrelationId(
            std::move(checkpointCorrelationId), observation.clamped);

        auto const& events = history.Events();
        observation.retainedEventCount = static_cast<std::uint32_t>(events.size());

        for (SimulationRunEvent const& event : events)
        {
            if (event.type == SimulationRunEventType::Progress)
                ++observation.progressEventCount;
            observation.clamped = observation.clamped || event.clamped;
        }

        if (events.empty())
            return observation;

        SimulationRunEvent const& latest = events.back();
        observation.simulationMinute = latest.simulationMinute;
        observation.progressBasisPoints = std::min<std::uint32_t>(latest.progressBasisPoints, 10000);
        observation.latestEventType = latest.type;
        observation.stopReason = latest.stopReason;
        observation.terminal = IsTerminal(latest.type);
        return observation;
    }

    std::string SimulationRunObservationBuilder::SanitizeCorrelationId(std::string value, bool& clamped)
    {
        for (char& character : value)
        {
            unsigned char const code = static_cast<unsigned char>(character);
            bool const safe =
                (code >= 'a' && code <= 'z') ||
                (code >= 'A' && code <= 'Z') ||
                (code >= '0' && code <= '9') ||
                character == '-' || character == '_' || character == '.';
            if (!safe)
            {
                character = '_';
                clamped = true;
            }
        }

        if (value.size() > SimulationRunObservationLimits::MaximumCheckpointCorrelationLength)
        {
            value.resize(SimulationRunObservationLimits::MaximumCheckpointCorrelationLength);
            clamped = true;
        }

        return value;
    }

    bool SimulationRunObservationBuilder::IsTerminal(SimulationRunEventType type)
    {
        return type == SimulationRunEventType::Completed ||
            type == SimulationRunEventType::Cancelled ||
            type == SimulationRunEventType::Failed;
    }
}
