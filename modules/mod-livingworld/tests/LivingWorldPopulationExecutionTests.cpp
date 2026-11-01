#include "LivingWorldPopulationExecution.h"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

using namespace LivingWorld;

namespace
{
    PopulationDecision MakeDecision(PopulationAction action, std::uint32_t requested)
    {
        PopulationDecision decision;
        decision.action = action;
        decision.requestedCount = requested;
        return decision;
    }

    PopulationCandidate MakeOnline(std::uint32_t guid)
    {
        PopulationCandidate candidate;
        candidate.characterGuid = guid;
        candidate.online = true;
        return candidate;
    }
}

int main()
{
    PlayerbotPopulationLimits const limits = { 10, 500 };

    // Login raises the target by exactly the rate-limited request.
    PopulationExecutionPlan login = PopulationExecutionPlanner::Plan(
        MakeDecision(PopulationAction::Login, 5), 100, 100, limits, {});
    assert(login.botCountTarget == 105);
    assert(login.writeBotCountTarget);
    assert(!login.clampedToPlayerbotLimits);
    assert(login.logoutGuids.empty());

    // No action holds the target at the current online count.
    PopulationExecutionPlan hold = PopulationExecutionPlanner::Plan(
        MakeDecision(PopulationAction::None, 0), 100, 100, limits, {});
    assert(hold.botCountTarget == 100);
    assert(!hold.writeBotCountTarget);

    // The manager re-randomizes out-of-range targets, so targets are clamped.
    PopulationExecutionPlan clampedHigh = PopulationExecutionPlanner::Plan(
        MakeDecision(PopulationAction::Login, 20), 495, 495, limits, {});
    assert(clampedHigh.botCountTarget == 500);
    assert(clampedHigh.clampedToPlayerbotLimits);

    PopulationExecutionPlan clampedLow = PopulationExecutionPlanner::Plan(
        MakeDecision(PopulationAction::None, 0), 3, 3, limits, {});
    assert(clampedLow.botCountTarget == 10);
    assert(clampedLow.clampedToPlayerbotLimits);
    assert(clampedLow.writeBotCountTarget);

    // Logout selects only unprotected bots and lowers the target by that many.
    std::vector<PopulationCandidate> candidates;
    candidates.push_back(MakeOnline(1));
    candidates.push_back(MakeOnline(2));
    candidates.back().inCombat = true;
    candidates.push_back(MakeOnline(3));
    candidates.back().sessionComplete = true;
    candidates.push_back(MakeOnline(4));
    candidates.back().interactingWithHuman = true;

    PopulationExecutionPlan logout = PopulationExecutionPlanner::Plan(
        MakeDecision(PopulationAction::Logout, 3), 100, 100, limits, candidates);
    assert(logout.logoutGuids.size() == 2);
    assert(logout.logoutGuids[0] == 3);
    assert(logout.logoutGuids[1] == 1);
    assert(logout.botCountTarget == 98);
    assert(logout.writeBotCountTarget);

    // When every bot is protected nothing leaves and the target is held.
    std::vector<PopulationCandidate> protectedOnly;
    protectedOnly.push_back(MakeOnline(7));
    protectedOnly.back().protectedActivity = true;
    PopulationExecutionPlan blocked = PopulationExecutionPlanner::Plan(
        MakeDecision(PopulationAction::Logout, 2), 100, 100, limits, protectedOnly);
    assert(blocked.logoutGuids.empty());
    assert(blocked.botCountTarget == 100);
    assert(!blocked.writeBotCountTarget);

    // Inverted Playerbots limits never produce a target below the minimum.
    PopulationExecutionPlan inverted = PopulationExecutionPlanner::Plan(
        MakeDecision(PopulationAction::None, 0), 50, 0, { 80, 20 }, {});
    assert(inverted.botCountTarget == 80);

    std::cout << "LivingWorld population execution tests passed.\n";
    return 0;
}
