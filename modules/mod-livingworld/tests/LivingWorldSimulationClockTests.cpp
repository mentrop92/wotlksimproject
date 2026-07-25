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
    clock.SetPaused(false);

    assert(!clock.SetMultiplier(0));
    assert(!clock.SetMultiplier(SimulationClockLimits::MaximumMultiplier + 1));
    assert(clock.GetSnapshot().multiplier == 60);

    clock.Reset(SimulationClockLimits::MaximumSimulationMinute - 1);
    assert(clock.SetMultiplier(SimulationClockLimits::MaximumMultiplier));
    assert(clock.Advance(60000) == 1);
    SimulationClockSnapshot capped = clock.GetSnapshot();
    assert(capped.simulationMinute == SimulationClockLimits::MaximumSimulationMinute);
    assert(capped.clamped);
    assert(clock.Advance(60000) == 0);

    clock.Reset(SimulationClockLimits::MaximumSimulationMinute + 100);
    assert(clock.GetSnapshot().simulationMinute == SimulationClockLimits::MaximumSimulationMinute);
    assert(clock.GetSnapshot().clamped);

    std::cout << "LivingWorld simulation clock tests passed.\n";
    return 0;
}
