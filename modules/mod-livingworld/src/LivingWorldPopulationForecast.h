#ifndef MOD_LIVINGWORLD_POPULATION_FORECAST_H
#define MOD_LIVINGWORLD_POPULATION_FORECAST_H

#include "LivingWorldLoadGovernor.h"
#include "LivingWorldSettings.h"

#include <cstdint>
#include <vector>

namespace LivingWorld
{
    struct PopulationForecastPoint
    {
        std::uint8_t dayOfWeek = 0;
        std::uint8_t hour = 0;
        std::uint32_t scheduledTarget = 0;
        std::uint32_t governedTarget = 0;
        std::uint32_t effectiveAICeiling = 0;
        std::uint32_t finalTarget = 0;
        LoadPressure loadPressure = LoadPressure::Healthy;
    };

    class PopulationForecast
    {
    public:
        static std::vector<PopulationForecastPoint> Build24Hours(
            std::uint8_t startDayOfWeek,
            std::uint8_t startHour,
            std::uint32_t expectedHumanOnline,
            WorldSettings const& settings,
            ServerLoadSnapshot const& load,
            LoadGovernorPolicy const& loadPolicy = {});
    };
}

#endif
