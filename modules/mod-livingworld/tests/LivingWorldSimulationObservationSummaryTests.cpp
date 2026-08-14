#include "LivingWorldSimulationObservationSummary.h"

#include <cassert>
#include <iostream>

using namespace LivingWorld;

namespace
{
    SimulationRunObservation Observation(
        std::uint64_t minute,
        std::uint32_t progress,
        std::string correlation = {},
        bool terminal = false,
        bool clamped = false)
    {
        SimulationRunObservation observation;
        observation.simulationMinute = minute;
        observation.progressBasisPoints = progress;
        observation.checkpointCorrelationId = std::move(correlation);
        observation.terminal = terminal;
        observation.clamped = clamped;
        return observation;
    }
}

int main()
{
    SimulationObservationHistory emptyHistory(8);
    SimulationObservationSummary empty = SimulationObservationSummarizer::Summarize(emptyHistory);
    assert(empty.retainedObservationCount == 0);
    assert(empty.coveredSimulationMinutes == 0);
    assert(!SimulationObservationSummarizer::DiagnoseCorrelation(emptyHistory, "checkpoint-a").has_value());
    assert(!SimulationObservationSummarizer::DiagnoseCorrelation(emptyHistory, "").has_value());

    SimulationObservationHistory history(8);
    assert(history.Append(Observation(100, 500, "checkpoint-a")));
    assert(history.Append(Observation(130, 2500, "checkpoint-b", false, true)));
    assert(history.Append(Observation(160, 7000, "checkpoint-a")));
    assert(history.Append(Observation(220, 10000, "checkpoint-a", true)));
    assert(history.Append(Observation(250, 9000)));

    SimulationObservationSummary summary = SimulationObservationSummarizer::Summarize(history);
    assert(summary.retainedObservationCount == 5);
    assert(summary.terminalObservationCount == 1);
    assert(summary.clampedObservationCount == 1);
    assert(summary.correlatedObservationCount == 4);
    assert(summary.uniqueCorrelationCount == 2);
    assert(summary.firstSimulationMinute == 100);
    assert(summary.latestSimulationMinute == 250);
    assert(summary.coveredSimulationMinutes == 150);
    assert(summary.minimumProgressBasisPoints == 500);
    assert(summary.maximumProgressBasisPoints == 10000);
    assert(summary.historyClamped);

    auto diagnostic = SimulationObservationSummarizer::DiagnoseCorrelation(history, "checkpoint-a");
    assert(diagnostic.has_value());
    assert(diagnostic->checkpointCorrelationId == "checkpoint-a");
    assert(diagnostic->observationCount == 3);
    assert(diagnostic->firstSimulationMinute == 100);
    assert(diagnostic->latestSimulationMinute == 220);
    assert(diagnostic->latestProgressBasisPoints == 10000);
    assert(diagnostic->latestTerminal);
    assert(!diagnostic->anyClamped);

    auto clampedDiagnostic = SimulationObservationSummarizer::DiagnoseCorrelation(history, "checkpoint-b");
    assert(clampedDiagnostic.has_value());
    assert(clampedDiagnostic->observationCount == 1);
    assert(clampedDiagnostic->anyClamped);
    assert(!SimulationObservationSummarizer::DiagnoseCorrelation(history, "missing").has_value());

    SimulationObservationHistory evicted(2);
    assert(evicted.Append(Observation(10, 100, "old")));
    assert(evicted.Append(Observation(20, 200, "kept")));
    assert(evicted.Append(Observation(30, 300, "kept")));
    SimulationObservationSummary evictedSummary = SimulationObservationSummarizer::Summarize(evicted);
    assert(evictedSummary.retainedObservationCount == 2);
    assert(evictedSummary.firstSimulationMinute == 20);
    assert(evictedSummary.uniqueCorrelationCount == 1);
    assert(!SimulationObservationSummarizer::DiagnoseCorrelation(evicted, "old").has_value());

    std::cout << "LivingWorld simulation observation-summary tests passed.\n";
    return 0;
}