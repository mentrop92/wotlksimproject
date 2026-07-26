#include "LivingWorldSimulationRunState.h"

#include <cassert>
#include <iostream>

using namespace LivingWorld;

int main()
{
    SimulationRunRequest request;
    request.startMinute = 100;
    request.durationMinutes = 125;
    request.tickMinutes = 60;
    request.multiplier = 10;

    SimulationRunPlan const plan = SimulationRunPlanner::Build(request);
    assert(plan.valid);

    SimulationRunState state = SimulationRunState::Create(plan);
    assert(state.GetSnapshot().status == SimulationRunStatus::Pending);
    assert(state.Start());
    assert(!state.Start());

    assert(state.Advance(60));
    assert(state.GetSnapshot().currentMinute == 160);
    assert(state.GetSnapshot().completedIterations == 1);
    assert(state.GetSnapshot().progressBasisPoints == 4800);

    assert(state.Advance(60));
    assert(state.GetSnapshot().currentMinute == 220);
    assert(state.GetSnapshot().completedIterations == 2);
    assert(state.GetSnapshot().progressBasisPoints == 9600);

    assert(state.Advance(60));
    assert(state.IsTerminal());
    assert(state.GetSnapshot().status == SimulationRunStatus::Completed);
    assert(state.GetSnapshot().currentMinute == 225);
    assert(state.GetSnapshot().completedIterations == plan.iterationCount);
    assert(state.GetSnapshot().progressBasisPoints == 10000);
    assert(!state.Advance(1));
    assert(!state.Cancel());

    SimulationRunState cancelled = SimulationRunState::Create(plan);
    assert(cancelled.Start());
    assert(cancelled.Advance(30));
    assert(cancelled.Cancel());
    assert(cancelled.GetSnapshot().status == SimulationRunStatus::Cancelled);
    assert(cancelled.GetSnapshot().stopReason == SimulationStopReason::UserRequested);
    assert(!cancelled.Advance(30));

    SimulationRunState failed = SimulationRunState::Create(plan);
    assert(failed.Start());
    assert(failed.Fail(SimulationStopReason::SafetyLimit));
    assert(failed.GetSnapshot().status == SimulationRunStatus::Failed);
    assert(failed.GetSnapshot().stopReason == SimulationStopReason::SafetyLimit);

    SimulationRunPlan invalidPlan;
    SimulationRunState invalid = SimulationRunState::Create(invalidPlan);
    assert(invalid.IsTerminal());
    assert(invalid.GetSnapshot().status == SimulationRunStatus::Failed);
    assert(invalid.GetSnapshot().stopReason == SimulationStopReason::InvalidPlan);
    assert(!invalid.Start());

    SimulationRunState zeroStep = SimulationRunState::Create(plan);
    assert(zeroStep.Start());
    assert(!zeroStep.Advance(0));
    assert(zeroStep.GetSnapshot().currentMinute == plan.startMinute);

    // A caller cannot bypass the planner's iteration ceiling by advancing in
    // smaller increments than the planned tick size.
    SimulationRunState exhausted = SimulationRunState::Create(plan);
    assert(exhausted.Start());
    assert(exhausted.Advance(1));
    assert(exhausted.Advance(1));
    assert(exhausted.Advance(1));
    assert(exhausted.IsTerminal());
    assert(exhausted.GetSnapshot().status == SimulationRunStatus::Failed);
    assert(exhausted.GetSnapshot().stopReason == SimulationStopReason::SafetyLimit);
    assert(exhausted.GetSnapshot().currentMinute == 103);
    assert(exhausted.GetSnapshot().completedIterations == plan.iterationCount);
    assert(exhausted.GetSnapshot().clamped);
    assert(!exhausted.Advance(1));

    std::cout << "LivingWorld simulation run-state tests passed.\n";
    return 0;
}
