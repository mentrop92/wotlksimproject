#include "LivingWorldPopulationForecast.h"

#include <cassert>
#include <iostream>

using namespace LivingWorld;

int main()
{
    WorldSettings settings;
    settings.minimumOnline = 50;
    settings.targetOnline = 250;
    settings.maximumOnline = 500;
    settings.maximumTotalOnline = 600;
    settings.reservedHumanSlots = 100;

    ServerLoadSnapshot healthy;
    auto forecast = PopulationForecast::Build24Hours(4, 18, 80, settings, healthy);
    assert(forecast.size() == 24);
    assert(forecast.front().dayOfWeek == 4);
    assert(forecast.front().hour == 18);
    assert(forecast[6].dayOfWeek == 5);
    assert(forecast[6].hour == 0);

    for (PopulationForecastPoint const& point : forecast)
    {
        assert(point.scheduledTarget >= settings.minimumOnline);
        assert(point.scheduledTarget <= settings.maximumOnline);
        assert(point.governedTarget <= point.scheduledTarget);
        assert(point.finalTarget <= point.governedTarget);
        assert(point.finalTarget <= point.effectiveAICeiling);
        assert(!point.loadConstrained);
        assert(!point.emergencyDrain);
    }

    // Human demand above the reserve independently constrains the final AI target.
    auto humanPressure = PopulationForecast::Build24Hours(0, 0, 550, settings, healthy);
    for (PopulationForecastPoint const& point : humanPressure)
    {
        assert(point.effectiveAICeiling == 50);
        assert(point.finalTarget <= 50);
    }

    // Critical server pressure marks every point as an emergency drain and lowers targets.
    ServerLoadSnapshot critical;
    critical.averageWorldUpdateMs = 200;
    auto criticalForecast = PopulationForecast::Build24Hours(0, 0, 0, settings, critical);
    for (PopulationForecastPoint const& point : criticalForecast)
    {
        assert(point.loadConstrained);
        assert(point.emergencyDrain);
        assert(point.governedTarget <= point.scheduledTarget);
    }

    std::cout << "LivingWorld population forecast tests passed.\n";
    return 0;
}
