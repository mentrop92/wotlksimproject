#ifndef MOD_LIVINGWORLD_SIMULATION_CLOCK_H
#define MOD_LIVINGWORLD_SIMULATION_CLOCK_H

#include <cstdint>

namespace LivingWorld
{
    namespace SimulationClockLimits
    {
        inline constexpr std::uint32_t MaximumMultiplier = 1000;
        inline constexpr std::uint64_t MaximumSimulationMinute = 525600000ULL;
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
        SimulationClockSnapshot GetSnapshot() const;

    private:
        std::uint64_t _simulationMinute = 0;
        std::uint64_t _fractionalCredit = 0;
        std::uint32_t _multiplier = 1;
        bool _paused = false;
        bool _clamped = false;
    };
}

#endif
