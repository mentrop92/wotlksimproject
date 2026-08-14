#include "LivingWorldSimulationObservationSummary.h"

#include <algorithm>
#include <unordered_set>

namespace LivingWorld
{
    SimulationObservationSummary SimulationObservationSummarizer::Summarize(
        SimulationObservationHistory const& history)
    {
        SimulationObservationSummary summary;
        summary.historyClamped = history.Clamped();

        auto const& observations = history.Observations();
        if (observations.empty())
            return summary;

        summary.retainedObservationCount = static_cast<std::uint32_t>(observations.size());
        summary.firstSimulationMinute = observations.front().simulationMinute;
        summary.latestSimulationMinute = observations.back().simulationMinute;
        summary.coveredSimulationMinutes = summary.latestSimulationMinute - summary.firstSimulationMinute;
        summary.minimumProgressBasisPoints = observations.front().progressBasisPoints;
        summary.maximumProgressBasisPoints = observations.front().progressBasisPoints;

        std::unordered_set<std::string> correlations;
        correlations.reserve(observations.size());

        for (SimulationRunObservation const& observation : observations)
        {
            summary.terminalObservationCount += observation.terminal ? 1U : 0U;
            summary.clampedObservationCount += observation.clamped ? 1U : 0U;
            summary.minimumProgressBasisPoints = std::min(
                summary.minimumProgressBasisPoints,
                observation.progressBasisPoints);
            summary.maximumProgressBasisPoints = std::max(
                summary.maximumProgressBasisPoints,
                observation.progressBasisPoints);

            if (!observation.checkpointCorrelationId.empty())
            {
                ++summary.correlatedObservationCount;
                correlations.insert(observation.checkpointCorrelationId);
            }
        }

        summary.uniqueCorrelationCount = static_cast<std::uint32_t>(correlations.size());
        return summary;
    }

    std::optional<SimulationCorrelationDiagnostic> SimulationObservationSummarizer::DiagnoseCorrelation(
        SimulationObservationHistory const& history,
        std::string const& checkpointCorrelationId)
    {
        if (checkpointCorrelationId.empty())
            return std::nullopt;

        SimulationCorrelationDiagnostic diagnostic;
        diagnostic.checkpointCorrelationId = checkpointCorrelationId;

        for (SimulationRunObservation const& observation : history.Observations())
        {
            if (observation.checkpointCorrelationId != checkpointCorrelationId)
                continue;

            if (diagnostic.observationCount == 0)
                diagnostic.firstSimulationMinute = observation.simulationMinute;

            ++diagnostic.observationCount;
            diagnostic.latestSimulationMinute = observation.simulationMinute;
            diagnostic.latestProgressBasisPoints = observation.progressBasisPoints;
            diagnostic.latestTerminal = observation.terminal;
            diagnostic.anyClamped = diagnostic.anyClamped || observation.clamped;
        }

        if (diagnostic.observationCount == 0)
            return std::nullopt;
        return diagnostic;
    }
}