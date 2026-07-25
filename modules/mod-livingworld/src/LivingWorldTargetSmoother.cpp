#include "LivingWorldTargetSmoother.h"

#include <algorithm>

namespace LivingWorld
{
    void TargetSmoother::Reset(std::uint32_t target)
    {
        _currentTarget = target;
        _recoveryHoldRemainingMs = 0;
        _riseCredit = 0;
        _fallCredit = 0;
        _initialized = true;
    }

    TargetSmoothingResult TargetSmoother::Update(
        std::uint32_t diffMs,
        std::uint32_t requestedTarget,
        bool pressureActive,
        bool emergencyDrain,
        TargetSmoothingPolicy const& policy)
    {
        if (!_initialized)
            Reset(requestedTarget);

        TargetSmoothingResult result;
        result.requestedTarget = requestedTarget;

        if (pressureActive)
            _recoveryHoldRemainingMs = policy.recoveryHoldMs;
        else if (_recoveryHoldRemainingMs > diffMs)
            _recoveryHoldRemainingMs -= diffMs;
        else
            _recoveryHoldRemainingMs = 0;

        if (requestedTarget > _currentTarget)
        {
            if (_recoveryHoldRemainingMs > 0)
            {
                result.recoveryHeld = true;
            }
            else
            {
                std::uint32_t const budget = AccumulateBudget(
                    _riseCredit,
                    diffMs,
                    std::max<std::uint32_t>(policy.maximumRisePerMinute, 1));
                std::uint32_t const increase = std::min(requestedTarget - _currentTarget, budget);
                _currentTarget += increase;
                result.riseLimited = _currentTarget < requestedTarget;
            }
        }
        else if (requestedTarget < _currentTarget)
        {
            if (emergencyDrain)
            {
                _currentTarget = requestedTarget;
                _fallCredit = 0;
            }
            else
            {
                std::uint32_t const budget = AccumulateBudget(
                    _fallCredit,
                    diffMs,
                    std::max<std::uint32_t>(policy.maximumFallPerMinute, 1));
                std::uint32_t const decrease = std::min(_currentTarget - requestedTarget, budget);
                _currentTarget -= decrease;
                result.fallLimited = _currentTarget > requestedTarget;
            }
        }

        result.smoothedTarget = _currentTarget;
        return result;
    }

    std::uint32_t TargetSmoother::CurrentTarget() const
    {
        return _currentTarget;
    }

    std::uint32_t TargetSmoother::AccumulateBudget(
        std::uint64_t& credit,
        std::uint32_t diffMs,
        std::uint32_t ratePerMinute)
    {
        credit += static_cast<std::uint64_t>(diffMs) * ratePerMinute;
        std::uint32_t const available = static_cast<std::uint32_t>(credit / 60000ULL);
        credit %= 60000ULL;
        return available;
    }
}
