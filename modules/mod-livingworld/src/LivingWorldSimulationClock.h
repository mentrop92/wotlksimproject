#ifndef MOD_LIVINGWORLD_SIMULATION_CLOCK_H
#define MOD_LIVINGWORLD_SIMULATION_CLOCK_H

#include <cstdint>

namespace LivingWorld
{
    namespace SimulationClockLimits
    {
        inline constexpr std::uint32_t MaximumMultiplier = 1000;
        inline constexpr std::uint64_t MaximumSimulationMinute = 525600000ULL;
        inline constexpr std::uint32_t MaximumStepMinutes = 10080;
        inline constexpr std::uint32_t MaximumStepHours = 168;
        inline constexpr std::uint32_t MaximumStepDays = 7;
    }

    struct SimulationClockSnapshot
    {
        std::uint64_t simulationMinute = 0;
        std::uint32_t multiplier = 1;
        bool paused = false;
        bool clamped = false;
    };

    class SimulationClock
    {
    public:
        void Reset(std::uint64_t simulationMinute = 0);
        bool SetMultiplier(std::uint32_t multiplier);
        void SetPaused(bool paused);
        std::uint64_t Advance(std::uint32_t realMilliseconds);
        std::uint64_t StepMinutes(std::uint32_t minutes);
        std::uint64_t StepHours(std::uint32_t hours);
        std::uint64_t StepDays(std::uint32_t days);
        SimulationClockSnapshot GetSnapshot() const;

    private:
        std::uint64_t ApplyStep(std::uint64_t minutes);

        std::uint64_t _simulationMinute = 0;
        std::uint64_t _fractionalCredit = 0;
        std::uint32_t _multiplier = 1;
        bool _paused = false;
        bool _clamped = false;
    };
}

#endif