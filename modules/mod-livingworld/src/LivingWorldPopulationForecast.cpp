#include "LivingWorldPopulationForecast.h"

#include "LivingWorldPopulationCurve.h"

#include <algorithm>

namespace LivingWorld
{
    std::vector<PopulationForecastPoint> PopulationForecast::Build24Hours(
        std::uint8_t startDayOfWeek,
        std::uint8_t startHour,
        std::uint32_t expectedHumanOnline,
        WorldSettings const& settings,
        ServerLoadSnapshot const& load,
        ServerLoadPolicy const& loadPolicy)
    {
        std::vector<PopulationForecastPoint> forecast;
        forecast.reserve(24);

        std::uint8_t day = static_cast<std::uint8_t>(startDayOfWeek % 7);
        std::uint8_t hour = static_cast<std::uint8_t>(startHour % 24);

        for (std::uint32_t index = 0; index < 24; ++index)
        {
            PopulationCurveInput curveInput;
            curveInput.minimumOnline = settings.minimumOnline;
            curveInput.baseTarget = settings.targetOnline;
            curveInput.maximumOnline = settings.maximumOnline;
            curveInput.weekday = day;
            curveInput.minuteOfDay = static_cast<std::uint16_t>(hour * 60U);

            PopulationCurveResult const curve = PopulationCurve::Evaluate(curveInput);
            ServerLoadDecision const governed = LoadGovernor::Evaluate(curve.target, load, loadPolicy);

            std::uint32_t const totalCapacity = std::min(settings.maximumTotalOnline, SafetyLimits::MaximumTotalOnline);
            std::uint32_t const protectedHumanSlots = std::max(expectedHumanOnline, settings.reservedHumanSlots);
            std::uint32_t const aiCeiling = protectedHumanSlots >= totalCapacity
                ? 0
                : std::min(settings.maximumOnline, totalCapacity - protectedHumanSlots);

            forecast.push_back({
                day,
                hour,
                curve.target,
                governed.governedTarget,
                aiCeiling,
                std::min(governed.governedTarget, aiCeiling),
                governed.constrained,
                governed.emergencyDrain
            });

            hour = static_cast<std::uint8_t>((hour + 1) % 24);
            if (hour == 0)
                day = static_cast<std::uint8_t>((day + 1) % 7);
        }

        return forecast;
    }
}
