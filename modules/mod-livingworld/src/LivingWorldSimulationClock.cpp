#include "LivingWorldSimulationClock.h"

#include <algorithm>
#include <limits>

namespace LivingWorld
{
    void SimulationClock::Reset(std::uint64_t simulationMinute)
    {
        _simulationMinute = std::min(simulationMinute, SimulationClockLimits::MaximumSimulationMinute);
        _fractionalCredit = 0;
        _multiplier = 1;
        _paused = false;
        _clamped = simulationMinute > SimulationClockLimits::MaximumSimulationMinute;
    }

    bool SimulationClock::SetMultiplier(std::uint32_t multiplier)
    {
        if (multiplier == 0 || multiplier > SimulationClockLimits::MaximumMultiplier)
            return false;

        _multiplier = multiplier;
        return true;
    }

    void SimulationClock::SetPaused(bool paused)
    {
        _paused = paused;
    }

    std::uint64_t SimulationClock::Advance(std::uint32_t realMilliseconds)
    {
        if (_paused || realMilliseconds == 0 || _simulationMinute >= SimulationClockLimits::MaximumSimulationMinute)
            return 0;

        constexpr std::uint64_t MillisecondsPerMinute = 60000ULL;
        std::uint64_t scaled = static_cast<std::uint64_t>(realMilliseconds) * _multiplier;
        if (scaled > std::numeric_limits<std::uint64_t>::max() - _fractionalCredit)
        {
            scaled = std::numeric_limits<std::uint64_t>::max() - _fractionalCredit;
            _clamped = true;
        }

        _fractionalCredit += scaled;
        std::uint64_t elapsedMinutes = _fractionalCredit / MillisecondsPerMinute;
        _fractionalCredit %= MillisecondsPerMinute;
        return ApplyStep(elapsedMinutes);
    }

    std::uint64_t SimulationClock::StepMinutes(std::uint32_t minutes)
    {
        if (minutes == 0 || minutes > SimulationClockLimits::MaximumStepMinutes)
            return 0;
        return ApplyStep(minutes);
    }

    std::uint64_t SimulationClock::StepHours(std::uint32_t hours)
    {
        if (hours == 0 || hours > SimulationClockLimits::MaximumStepHours)
            return 0;
        return ApplyStep(static_cast<std::uint64_t>(hours) * 60ULL);
    }

    std::uint64_t SimulationClock::StepDays(std::uint32_t days)
    {
        if (days == 0 || days > SimulationClockLimits::MaximumStepDays)
            return 0;
        return ApplyStep(static_cast<std::uint64_t>(days) * 1440ULL);
    }

    std::uint64_t SimulationClock::ApplyStep(std::uint64_t minutes)
    {
        if (minutes == 0 || _simulationMinute >= SimulationClockLimits::MaximumSimulationMinute)
            return 0;

        std::uint64_t const remaining = SimulationClockLimits::MaximumSimulationMinute - _simulationMinute;
        std::uint64_t const applied = std::min(minutes, remaining);
        _simulationMinute += applied;

        if (applied < minutes)
        {
            _fractionalCredit = 0;
            _clamped = true;
        }

        return applied;
    }

    SimulationClockSnapshot SimulationClock::GetSnapshot() const
    {
        return { _simulationMinute, _multiplier, _paused, _clamped };
    }
}