#include "LivingWorldSimulationClock.h"

#include <cassert>
#include <iostream>

using namespace LivingWorld;

int main()
{
    SimulationClock clock;
    clock.Reset();
    assert(clock.GetSnapshot().simulationMinute == 0);
    assert(clock.SetMultiplier(60));
    assert(clock.Advance(1000) == 1);
    assert(clock.GetSnapshot().simulationMinute == 1);

    clock.SetPaused(true);
    assert(clock.Advance(60000) == 0);
    assert(clock.GetSnapshot().simulationMinute == 1);

    // Explicit administrative steps remain deterministic while paused, but
    // each request is bounded to at most seven simulated days.
    assert(clock.StepMinutes(59) == 59);
    assert(clock.StepHours(2) == 120);
    assert(clock.StepDays(1) == 1440);
    assert(clock.GetSnapshot().simulationMinute == 1620);
    assert(clock.StepMinutes(0) == 0);
    assert(clock.StepMinutes(SimulationClockLimits::MaximumStepMinutes + 1) == 0);
    assert(clock.StepHours(SimulationClockLimits::MaximumStepHours + 1) == 0);
    assert(clock.StepDays(SimulationClockLimits::MaximumStepDays + 1) == 0);
    clock.SetPaused(false);

    assert(!clock.SetMultiplier(0));
    assert(!clock.SetMultiplier(SimulationClockLimits::MaximumMultiplier + 1));
    assert(clock.GetSnapshot().multiplier == 60);

    // Fractional real-time credit is retained deterministically.
    clock.Reset();
    assert(clock.SetMultiplier(1));
    assert(clock.Advance(30000) == 0);
    assert(clock.Advance(30000) == 1);

    // Repeated maximum legal steps produce a deterministic long horizon.
    clock.Reset();
    for (std::uint32_t week = 0; week < 52; ++week)
        assert(clock.StepDays(7) == 10080);
    assert(clock.GetSnapshot().simulationMinute == 524160);

    clock.Reset(SimulationClockLimits::MaximumSimulationMinute - 1);
    assert(clock.SetMultiplier(SimulationClockLimits::MaximumMultiplier));
    assert(clock.Advance(60000) == 1);
    SimulationClockSnapshot capped = clock.GetSnapshot();
    assert(capped.simulationMinute == SimulationClockLimits::MaximumSimulationMinute);
    assert(capped.clamped);
    assert(clock.Advance(60000) == 0);
    assert(clock.StepDays(1) == 0);

    clock.Reset(SimulationClockLimits::MaximumSimulationMinute - 10);
    assert(clock.StepHours(1) == 10);
    assert(clock.GetSnapshot().simulationMinute == SimulationClockLimits::MaximumSimulationMinute);
    assert(clock.GetSnapshot().clamped);

    clock.Reset(SimulationClockLimits::MaximumSimulationMinute + 100);
    assert(clock.GetSnapshot().simulationMinute == SimulationClockLimits::MaximumSimulationMinute);
    assert(clock.GetSnapshot().clamped);

    std::cout << "LivingWorld simulation clock tests passed.\n";
    return 0;
}