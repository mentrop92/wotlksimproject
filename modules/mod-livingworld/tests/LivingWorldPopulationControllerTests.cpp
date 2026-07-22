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

    controller.Reset();
    PopulationDecision decision = controller.Update(5000, 80, settings);
    assert(decision.action == PopulationAction::Login);
    assert(decision.requestedCount == 1);
    assert(decision.effectiveTarget == 100);
    assert(decision.lowerBound == 95);
    assert(decision.upperBound == 105);

    decision = controller.Update(5000, 80, settings);
    assert(decision.action == PopulationAction::Login);
    assert(decision.requestedCount == 1);

    controller.Reset();
    decision = controller.Update(10000, 130, settings);
    assert(decision.action == PopulationAction::Logout);
    assert(decision.requestedCount == 1);

    controller.Reset();
    decision = controller.Update(60000, 97, settings);
    assert(decision.action == PopulationAction::None);
    assert(decision.requestedCount == 0);

    controller.Reset();
    decision = controller.Update(60000, 104, settings);
    assert(decision.action == PopulationAction::None);
    assert(decision.requestedCount == 0);

    settings.loginEnabled = false;
    controller.Reset();
    decision = controller.Update(60000, 20, settings);
    assert(decision.action == PopulationAction::None);
    assert(decision.effectiveTarget == 20);
    assert(decision.lowerBound == 20);

    settings.loginEnabled = true;
    settings.simulationEnabled = false;
    controller.Reset();
    decision = controller.Update(60000, 150, settings);
    assert(decision.action == PopulationAction::None);

    settings.simulationEnabled = true;
    settings.autoScale = false;
    controller.Reset();
    decision = controller.Update(60000, 20, settings);
    assert(decision.action == PopulationAction::None);

    settings.autoScale = true;
    settings.targetOnline = 500;
    controller.Reset();
    decision = controller.Update(60000, 194, settings);
    assert(decision.effectiveTarget == 200);
    assert(decision.lowerBound == 195);
    assert(decision.upperBound == 200);
    assert(decision.action == PopulationAction::Login);
    assert(decision.requestedCount == 6);

    controller.Reset();
    decision = controller.Update(60000, 197, settings);
    assert(decision.action == PopulationAction::None);

    std::cout << "LivingWorld population controller tests passed.\n";
    return 0;
}
