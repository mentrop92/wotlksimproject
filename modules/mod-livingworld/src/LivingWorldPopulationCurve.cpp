#include "LivingWorldPopulationCurve.h"

#include <algorithm>
#include <cstdint>

namespace LivingWorld
{
    namespace
    {
        std::uint16_t TimeMultiplier(std::uint16_t minuteOfDay)
        {
            minuteOfDay = std::min<std::uint16_t>(minuteOfDay, 1439);

            if (minuteOfDay < 360)   // midnight–6 AM
                return 4500;
            if (minuteOfDay < 540)   // 6–9 AM
                return 6500;
            if (minuteOfDay < 720)   // 9 AM–noon
                return 8000;
            if (minuteOfDay < 1020)  // noon–5 PM
                return 9500;
            if (minuteOfDay < 1320)  // 5–10 PM
                return 13000;
            return 9000;             // 10 PM–midnight
        }
    }

    PopulationCurveResult PopulationCurve::Evaluate(PopulationCurveInput const& input)
    {
        PopulationCurveResult result;
        result.weekend = input.weekday >= 5;
        result.peakWindow = input.minuteOfDay >= 1020 && input.minuteOfDay < 1320;

        std::uint32_t multiplier = TimeMultiplier(input.minuteOfDay);
        if (result.weekend)
            multiplier = (multiplier * 115U) / 100U;

        result.multiplierBasisPoints = static_cast<std::uint16_t>(
            std::min<std::uint32_t>(multiplier, 20000U));

        std::uint64_t const scaled =
            static_cast<std::uint64_t>(input.baseTarget) * result.multiplierBasisPoints;
        std::uint32_t const rawTarget = static_cast<std::uint32_t>((scaled + 5000ULL) / 10000ULL);

        std::uint32_t const safeMaximum = std::max(input.minimumOnline, input.maximumOnline);
        result.target = std::clamp(rawTarget, input.minimumOnline, safeMaximum);
        return result;
    }
}
