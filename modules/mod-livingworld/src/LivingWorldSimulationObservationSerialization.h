#ifndef MOD_LIVINGWORLD_SIMULATION_OBSERVATION_SERIALIZATION_H
#define MOD_LIVINGWORLD_SIMULATION_OBSERVATION_SERIALIZATION_H

#include "LivingWorldSimulationObservationSummary.h"
#include <cstddef>
#include <optional>
#include <string>

namespace LivingWorld
{
class SimulationObservationSerializer
{
public:
    static constexpr std::size_t MaxSerializedBytes = 1024;
    static constexpr std::size_t MaxCorrelationIdBytes = 96;

    static std::optional<std::string> SerializeSummary(SimulationObservationSummary const& value);
    static std::optional<SimulationObservationSummary> DeserializeSummary(std::string const& payload);

    static std::optional<std::string> SerializeDiagnostic(SimulationCorrelationDiagnostic const& value);
    static std::optional<SimulationCorrelationDiagnostic> DeserializeDiagnostic(std::string const& payload);

private:
    static bool IsSafeCorrelationId(std::string const& value);
};
}

#endif
