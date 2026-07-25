#include "LivingWorldPopulationController.h"

#include <algorithm>

namespace LivingWorld
{
    PopulationController& PopulationController::Instance()
    {
        static PopulationController instance;
        return instance;
    }

    void PopulationController::Reset()
    {
        _loginCredit = 0;
        _logoutCredit = 0;
        _lastDecision = PopulationDecision{};
    }

    PopulationDecision PopulationController::Update(
        std::uint32_t diffMs,
        std::uint32_t currentOnline,
        std::uint32_t currentHumanOnline,
        WorldSettings const& settings)
    {
        PopulationDecision decision;
        decision.currentOnline = currentOnline;
        decision.currentHumanOnline = currentHumanOnline;
        decision.effectiveAICeiling = EffectiveAICeiling(currentHumanOnline, settings);
        decision.effectiveTarget = EffectiveTarget(currentOnline, decision.effectiveAICeiling, settings);
        CalculateBand(decision, settings);

        if (!settings.simulationEnabled || !settings.autoScale)
        {
            _lastDecision = decision;
            return decision;
        }

        if (currentOnline < decision.lowerBound && settings.loginEnabled)
        {
            std::uint32_t const budget = CalculateBudget(_loginCredit, diffMs, settings.loginRatePerMinute);
            decision.action = PopulationAction::Login;
            decision.requestedCount = std::min(decision.effectiveTarget - currentOnline, budget);
        }
        else if (currentOnline > decision.upperBound)
        {
            std::uint32_t const budget = CalculateBudget(_logoutCredit, diffMs, settings.logoutRatePerMinute);
            decision.action = PopulationAction::Logout;
            decision.requestedCount = std::min(currentOnline - decision.effectiveTarget, budget);
        }

        if (decision.requestedCount == 0)
            decision.action = PopulationAction::None;

        _lastDecision = decision;
        return decision;
    }

    PopulationDecision const& PopulationController::GetLastDecision() const
    {
        return _lastDecision;
    }

    std::uint32_t PopulationController::CalculateBudget(
        std::uint64_t& credit,
        std::uint32_t diffMs,
        std::uint32_t ratePerMinute)
    {
        credit += static_cast<std::uint64_t>(diffMs) * ratePerMinute;
        std::uint32_t const available = static_cast<std::uint32_t>(credit / 60000ULL);
        credit %= 60000ULL;
        return available;
    }

    std::uint32_t PopulationController::EffectiveAICeiling(
        std::uint32_t currentHumanOnline,
        WorldSettings const& settings)
    {
        std::uint32_t const totalCapacity = std::min(settings.maximumTotalOnline, SafetyLimits::MaximumTotalOnline);
        std::uint32_t const protectedHumanSlots = std::max(currentHumanOnline, settings.reservedHumanSlots);

        if (protectedHumanSlots >= totalCapacity)
            return 0;

        return std::min(settings.maximumOnline, totalCapacity - protectedHumanSlots);
    }

    std::uint32_t PopulationController::EffectiveTarget(
        std::uint32_t currentOnline,
        std::uint32_t effectiveAICeiling,
        WorldSettings const& settings)
    {
        std::uint32_t target = std::min(
            std::clamp(settings.targetOnline, settings.minimumOnline, settings.maximumOnline),
            effectiveAICeiling);

        if (!settings.loginEnabled && currentOnline < target)
            target = currentOnline;

        return target;
    }

    void PopulationController::CalculateBand(PopulationDecision& decision, WorldSettings const& settings)
    {
        if (!settings.loginEnabled && decision.currentOnline <= decision.effectiveTarget)
        {
            decision.lowerBound = decision.currentOnline;
            decision.upperBound = decision.effectiveTarget;
            return;
        }

        std::uint32_t const tolerance = std::min(
            settings.populationTolerance,
            settings.maximumOnline - settings.minimumOnline);

        std::uint32_t const policyMinimum = std::min(settings.minimumOnline, decision.effectiveAICeiling);
        decision.lowerBound = decision.effectiveTarget > tolerance
            ? std::max(policyMinimum, decision.effectiveTarget - tolerance)
            : policyMinimum;
        decision.upperBound = std::min(decision.effectiveAICeiling, decision.effectiveTarget + tolerance);
    }
}
