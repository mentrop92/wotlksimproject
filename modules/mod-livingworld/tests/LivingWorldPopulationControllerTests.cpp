#include "LivingWorldPopulationController.h"

#include <cassert>
#include <cstdint>
#include <iostream>

using namespace LivingWorld;

int main()
{
    PopulationController& controller = PopulationController::Instance();
    WorldSettings settings;
    settings.simulationEnabled = true;
    settings.loginEnabled = true;
    settings.autoScale = true;
    settings.minimumOnline = 50;
    settings.targetOnline = 100;
    settings.maximumOnline = 200;
    settings.populationTolerance = 5;
    settings.loginRatePerMinute = 12;
    settings.logoutRatePerMinute = 6;
    settings.maximumTotalOnline = 300;
    settings.reservedHumanSlots = 50;

    controller.Reset();
    PopulationDecision decision = controller.Update(5000, 80, 10, settings);
    assert(decision.action == PopulationAction::Login);
    assert(decision.requestedCount == 1);
    assert(decision.effectiveTarget == 100);
    assert(decision.effectiveAICeiling == 200);
    assert(decision.lowerBound == 95);
    assert(decision.upperBound == 105);

    decision = controller.Update(5000, 80, 10, settings);
    assert(decision.action == PopulationAction::Login);
    assert(decision.requestedCount == 1);

    controller.Reset();
    decision = controller.Update(10000, 130, 10, settings);
    assert(decision.action == PopulationAction::Logout);
    assert(decision.requestedCount == 1);

    controller.Reset();
    decision = controller.Update(60000, 97, 10, settings);
    assert(decision.action == PopulationAction::None);

    controller.Reset();
    decision = controller.Update(60000, 104, 10, settings);
    assert(decision.action == PopulationAction::None);

    settings.loginEnabled = false;
    controller.Reset();
    decision = controller.Update(60000, 20, 10, settings);
    assert(decision.action == PopulationAction::None);
    assert(decision.effectiveTarget == 20);
    assert(decision.lowerBound == 20);

    settings.loginEnabled = true;
    settings.simulationEnabled = false;
    controller.Reset();
    decision = controller.Update(60000, 150, 10, settings);
    assert(decision.action == PopulationAction::None);

    settings.simulationEnabled = true;
    settings.autoScale = false;
    controller.Reset();
    decision = controller.Update(60000, 20, 10, settings);
    assert(decision.action == PopulationAction::None);

    settings.autoScale = true;
    settings.targetOnline = 500;
    controller.Reset();
    decision = controller.Update(60000, 194, 10, settings);
    assert(decision.effectiveTarget == 200);
    assert(decision.lowerBound == 195);
    assert(decision.upperBound == 200);
    assert(decision.action == PopulationAction::Login);
    assert(decision.requestedCount == 6);

    controller.Reset();
    decision = controller.Update(60000, 197, 10, settings);
    assert(decision.action == PopulationAction::None);

    // Human demand above the reserve reduces the effective AI ceiling.
    settings.targetOnline = 180;
    settings.maximumOnline = 200;
    controller.Reset();
    decision = controller.Update(60000, 170, 160, settings);
    assert(decision.effectiveAICeiling == 140);
    assert(decision.effectiveTarget == 140);
    assert(decision.action == PopulationAction::Logout);
    assert(decision.requestedCount == 6);

    // When humans consume all capacity, the dry-run planner drains AI toward zero.
    controller.Reset();
    decision = controller.Update(60000, 20, 300, settings);
    assert(decision.effectiveAICeiling == 0);
    assert(decision.effectiveTarget == 0);
    assert(decision.action == PopulationAction::Logout);
    assert(decision.requestedCount == 6);

    std::cout << "LivingWorld population controller tests passed.\n";
    return 0;
}
