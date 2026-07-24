#include "LivingWorldPopulationPolicy.h"

#include <cassert>
#include <iostream>

using namespace LivingWorld;

int main()
{
    WorldSettings settings;
    settings.simulationEnabled = true;
    settings.loginEnabled = true;
    settings.autoScale = true;
    settings.minimumOnline = 50;
    settings.targetOnline = 200;
    settings.maximumOnline = 400;
    settings.populationTolerance = 5;
    settings.loginRatePerMinute = 60;
    settings.logoutRatePerMinute = 60;
    settings.maximumTotalOnline = 600;
    settings.reservedHumanSlots = 100;

    PopulationPolicyInput input;
    input.diffMs = 60000;
    input.currentAIOnline = 100;
    input.currentHumanOnline = 25;
    input.weekday = 0;
    input.minuteOfDay = 20 * 60;

    // Fixed mode preserves the configured target.
    settings.populationMode = PopulationMode::Fixed;
    sLivingWorldPopulation.Reset();
    PopulationPolicyResult result = PopulationPolicy::Evaluate(input, settings);
    assert(!result.scheduleApplied);
    assert(!result.loadApplied);
    assert(result.governedTarget == 200);
    assert(result.populationDecision.action == PopulationAction::Login);

    // Scheduled mode applies the evening curve without load governance.
    settings.populationMode = PopulationMode::Scheduled;
    sLivingWorldPopulation.Reset();
    result = PopulationPolicy::Evaluate(input, settings);
    assert(result.scheduleApplied);
    assert(!result.loadApplied);
    assert(result.scheduledTarget >= settings.targetOnline);
    assert(result.governedTarget == result.scheduledTarget);

    // Adaptive mode applies both the schedule and warning load reduction.
    settings.populationMode = PopulationMode::Adaptive;
    input.currentAIOnline = 250;
    input.load.averageWorldUpdateMs = 100;
    sLivingWorldPopulation.Reset();
    result = PopulationPolicy::Evaluate(input, settings);
    assert(result.scheduleApplied);
    assert(result.loadApplied);
    assert(!result.emergencyDrain);
    assert(result.governedTarget < result.scheduledTarget);
    assert(result.populationDecision.action == PopulationAction::Logout);

    // Critical pressure can safely yield below the persisted minimum in the
    // effective copy, requesting a bounded emergency drain without changing settings.
    input.load.averageWorldUpdateMs = 200;
    input.currentAIOnline = 200;
    sLivingWorldPopulation.Reset();
    result = PopulationPolicy::Evaluate(input, settings);
    assert(result.emergencyDrain);
    assert(result.governedTarget <= result.scheduledTarget / 2);
    assert(result.populationDecision.action == PopulationAction::Logout);
    assert(settings.minimumOnline == 50);
    assert(settings.targetOnline == 200);

    // Human capacity remains a final independent ceiling.
    input.load = {};
    input.currentHumanOnline = 590;
    input.currentAIOnline = 20;
    sLivingWorldPopulation.Reset();
    result = PopulationPolicy::Evaluate(input, settings);
    assert(result.populationDecision.effectiveAICeiling == 10);
    assert(result.populationDecision.action == PopulationAction::Logout);

    std::cout << "LivingWorld unified population policy tests passed.\n";
    return 0;
}
