#ifndef MOD_LIVINGWORLD_TARGET_SMOOTHER_H
#define MOD_LIVINGWORLD_TARGET_SMOOTHER_H

#include <cstdint>

namespace LivingWorld
{
    struct TargetSmoothingPolicy
    {
        std::uint32_t maximumRisePerMinute = 25;
        std::uint32_t maximumFallPerMinute = 100;
        std::uint32_t recoveryHoldMs = 120000;
    };

    struct TargetSmoothingResult
    {
        std::uint32_t requestedTarget = 0;
        std::uint32_t smoothedTarget = 0;
        bool riseLimited = false;
        bool fallLimited = false;
        bool recoveryHeld = false;
    };

    class TargetSmoother
    {
    public:
        void Reset(std::uint32_t target = 0);
        TargetSmoothingResult Update(
            std::uint32_t diffMs,
            std::uint32_t requestedTarget,
            bool pressureActive,
            bool emergencyDrain,
            TargetSmoothingPolicy const& policy = TargetSmoothingPolicy{});
        std::uint32_t CurrentTarget() const;

    private:
        static std::uint32_t AccumulateBudget(
            std::uint64_t& credit,
            std::uint32_t diffMs,
            std::uint32_t ratePerMinute);

        std::uint32_t _currentTarget = 0;
        std::uint32_t _recoveryHoldRemainingMs = 0;
        std::uint64_t _riseCredit = 0;
        std::uint64_t _fallCredit = 0;
        bool _initialized = false;
    };
}

#endif
