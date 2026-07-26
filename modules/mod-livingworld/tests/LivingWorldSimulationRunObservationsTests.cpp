#include "LivingWorldSimulationRunObservations.h"

#include <cassert>
#include <iostream>
#include <string>

using namespace LivingWorld;

int main()
{
    SimulationRunEventHistory history;

    SimulationRunObservation empty = SimulationRunObservationBuilder::Build(history, "checkpoint-0");
    assert(empty.retainedEventCount == 0);
    assert(!empty.terminal);
    assert(empty.checkpointCorrelationId == "checkpoint-0");

    SimulationRunEvent created;
    created.type = SimulationRunEventType::Created;
    created.simulationMinute = 100;
    assert(history.Append(created));

    SimulationRunEvent progress;
    progress.type = SimulationRunEventType::Progress;
    progress.simulationMinute = 160;
    progress.progressBasisPoints = 5000;
    assert(history.Append(progress));

    SimulationRunEvent completed;
    completed.type = SimulationRunEventType::Completed;
    completed.simulationMinute = 220;
    completed.progressBasisPoints = 10000;
    assert(history.Append(completed));

    SimulationRunObservation observation = SimulationRunObservationBuilder::Build(
        history, "checkpoint:unsafe/value");
    assert(observation.simulationMinute == 220);
    assert(observation.progressBasisPoints == 10000);
    assert(observation.retainedEventCount == 3);
    assert(observation.progressEventCount == 1);
    assert(observation.latestEventType == SimulationRunEventType::Completed);
    assert(observation.terminal);
    assert(observation.clamped);
    assert(observation.checkpointCorrelationId == "checkpoint_unsafe_value");

    std::string oversized(100, 'a');
    SimulationRunObservation bounded = SimulationRunObservationBuilder::Build(history, oversized);
    assert(bounded.checkpointCorrelationId.size() ==
        SimulationRunObservationLimits::MaximumCheckpointCorrelationLength);
    assert(bounded.clamped);

    SimulationRunEventHistory failedHistory;
    SimulationRunEvent failed;
    failed.type = SimulationRunEventType::Failed;
    failed.simulationMinute = 42;
    failed.stopReason = SimulationStopReason::InvalidPlan;
    failed.clamped = true;
    assert(failedHistory.Append(failed));

    SimulationRunObservation failedObservation = SimulationRunObservationBuilder::Build(failedHistory);
    assert(failedObservation.terminal);
    assert(failedObservation.stopReason == SimulationStopReason::InvalidPlan);
    assert(failedObservation.clamped);

    std::cout << "LivingWorld simulation run-observation tests passed.\n";
    return 0;
}
