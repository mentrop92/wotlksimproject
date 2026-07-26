#ifndef MOD_LIVINGWORLD_SIMULATION_OBSERVATION_SUMMARY_H
#define MOD_LIVINGWORLD_SIMULATION_OBSERVATION_SUMMARY_H

#include "LivingWorldSimulationObservationHistory.h"

#include <cstdint>
#include <optional>
#include <string>

namespace LivingWorld
{
    struct SimulationObservationSummary
    {
        std::uint32_t retainedObservationCount = 0;
        std::uint32_t terminalObservationCount = 0;
        std::uint32_t clampedObservationCount = 0;
        std::uint32_t correlatedObservationCount = 0;
        std::uint32_t uniqueCorrelationCount = 0;
        std::uint64_t firstSimulationMinute = 0;
        std::uint64_t latestSimulationMinute = 0;
        std::uint64_t coveredSimulationMinutes = 0;
        std::uint32_t minimumProgressBasisPoints = 0;
        std::uint32_t maximumProgressBasisPoints = 0;
        bool historyClamped = false;
    };

    struct SimulationCorrelationDiagnostic
    {
        std::string checkpointCorrelationId;
        std::uint32_t observationCount = 0;
        std::uint64_t firstSimulationMinute = 0;
        std::uint64_t latestSimulationMinute = 0;
        std::uint32_t latestProgressBasisPoints = 0;
        bool latestTerminal = false;
        bool anyClamped = false;
    };

    class SimulationObservationSummarizer
    {
    public:
        static SimulationObservationSummary Summarize(SimulationObservationHistory const& history);
        static std::optional<SimulationCorrelationDiagnostic> DiagnoseCorrelation(
            SimulationObservationHistory const& history,
            std::string const& checkpointCorrelationId);
    };
}

#endif