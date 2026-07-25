#ifndef MOD_LIVINGWORLD_POPULATION_CURVE_H
#define MOD_LIVINGWORLD_POPULATION_CURVE_H

#include <cstdint>

namespace LivingWorld
{
    struct PopulationCurveInput
    {
        std::uint32_t minimumOnline = 0;
        std::uint32_t baseTarget = 0;
        std::uint32_t maximumOnline = 0;
        std::uint8_t weekday = 0;       // 0 = Monday, 6 = Sunday
        std::uint16_t minuteOfDay = 0;  // 0..1439
    };

    struct PopulationCurveResult
    {
        std::uint32_t target = 0;
        std::uint16_t multiplierBasisPoints = 10000;
        bool weekend = false;
        bool peakWindow = false;
    };

    class PopulationCurve
    {
    public:
        static PopulationCurveResult Evaluate(PopulationCurveInput const& input);
    };
}

#endif
