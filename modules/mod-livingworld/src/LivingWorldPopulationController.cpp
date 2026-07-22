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
        WorldSettings const& settings)
    {
        PopulationDecision decision;
        decision.currentOnline = currentOnline;
        decision.effectiveTarget = EffectiveTarget(currentOnline, settings);

        if (!settings.simulationEnabled || !settings.autoScale)
        {
            _lastDecision = decision;
            return decision;
        }

        if (currentOnline < decision.effectiveTarget && settings.loginEnabled)
        {
            std::uint32_t const budget = CalculateBudget(_loginCredit, diffMs, settings.loginRatePerMinute);
            decision.action = PopulationAction::Login;
            decision.requestedCount = std::min(decision.effectiveTarget - currentOnline, budget);
        }
        else if (currentOnline > decision.effectiveTarget)
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

    std::uint32_t PopulationController::EffectiveTarget(
        std::uint32_t currentOnline,
        WorldSettings const& settings)
    {
        std::uint32_t target = std::clamp(settings.targetOnline, settings.minimumOnline, settings.maximumOnline);

        if (!settings.loginEnabled && currentOnline < target)
            target = currentOnline;

        return target;
    }
}
