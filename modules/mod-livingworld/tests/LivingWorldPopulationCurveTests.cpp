#include "LivingWorldPopulationCurve.h"

#include <cassert>
#include <iostream>

using namespace LivingWorld;

int main()
{
    PopulationCurveInput input;
    input.minimumOnline = 50;
    input.baseTarget = 200;
    input.maximumOnline = 400;
    input.weekday = 1;

    input.minuteOfDay = 180;
    PopulationCurveResult overnight = PopulationCurve::Evaluate(input);
    assert(overnight.target == 90);
    assert(!overnight.weekend);
    assert(!overnight.peakWindow);

    input.minuteOfDay = 1140;
    PopulationCurveResult weekdayPeak = PopulationCurve::Evaluate(input);
    assert(weekdayPeak.target == 260);
    assert(weekdayPeak.peakWindow);

    input.weekday = 5;
    PopulationCurveResult weekendPeak = PopulationCurve::Evaluate(input);
    assert(weekendPeak.target == 299);
    assert(weekendPeak.weekend);

    input.baseTarget = 1000;
    PopulationCurveResult capped = PopulationCurve::Evaluate(input);
    assert(capped.target == 400);

    input.baseTarget = 10;
    input.minuteOfDay = 180;
    PopulationCurveResult floored = PopulationCurve::Evaluate(input);
    assert(floored.target == 50);

    std::cout << "LivingWorld population curve tests passed.\n";
    return 0;
}
