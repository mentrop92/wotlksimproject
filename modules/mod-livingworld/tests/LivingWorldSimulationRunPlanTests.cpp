#include "LivingWorldSimulationRunPlan.h"

#include <cassert>
#include <iostream>

using namespace LivingWorld;

int main()
{
    SimulationRunRequest request;
    request.startMinute = 100;
    request.durationMinutes = 125;
    request.tickMinutes = 60;
    request.multiplier = 20;

    SimulationRunPlan plan = SimulationRunPlanner::Build(request);
    assert(plan.valid);
    assert(!plan.clamped);
    assert(plan.startMinute == 100);
    assert(plan.endMinute == 225);
    assert(plan.iterationCount == 3);
    assert(plan.finalTickMinutes == 5);
    assert(plan.multiplier == 20);

    request.durationMinutes = 0;
    plan = SimulationRunPlanner::Build(request);
    assert(!plan.valid);

    request.startMinute = SimulationClockLimits::MaximumSimulationMinute - 10;
    request.durationMinutes = 100;
    request.tickMinutes = 4;
    request.multiplier = SimulationClockLimits::MaximumMultiplier + 10;
    plan = SimulationRunPlanner::Build(request);
    assert(plan.valid);
    assert(plan.clamped);
    assert(plan.durationMinutes == 10);
    assert(plan.endMinute == SimulationClockLimits::MaximumSimulationMinute);
    assert(plan.multiplier == SimulationClockLimits::MaximumMultiplier);

    request.startMinute = 0;
    request.durationMinutes = SimulationRunLimits::MaximumRequestedMinutes;
    request.tickMinutes = 1;
    request.multiplier = 1;
    plan = SimulationRunPlanner::Build(request);
    assert(plan.valid);
    assert(plan.clamped);
    assert(plan.iterationCount <= SimulationRunLimits::MaximumIterations);
    assert(plan.tickMinutes >= 6);

    request.durationMinutes = 60;
    request.tickMinutes = 0;
    plan = SimulationRunPlanner::Build(request);
    assert(plan.valid);
    assert(plan.clamped);
    assert(plan.tickMinutes == 1);
    assert(plan.iterationCount == 60);

    std::cout << "LivingWorld simulation run-plan tests passed.\n";
    return 0;
}
