#ifndef MOD_LIVINGWORLD_SIMULATION_RUN_PLAN_H
#define MOD_LIVINGWORLD_SIMULATION_RUN_PLAN_H

#include "LivingWorldSimulationClock.h"

#include <cstdint>

namespace LivingWorld
{
    namespace SimulationRunLimits
    {
        inline constexpr std::uint32_t MaximumRequestedMinutes = 525600;
        inline constexpr std::uint32_t MaximumIterations = 100000;
        inline constexpr std::uint32_t MaximumTickMinutes = SimulationClockLimits::MaximumStepMinutes;
    }

    struct SimulationRunRequest
    {
        std::uint64_t startMinute = 0;
        std::uint32_t durationMinutes = 0;
        std::uint32_t tickMinutes = 1;
        std::uint32_t multiplier = 1;
    };

    struct SimulationRunPlan
    {
        std::uint64_t startMinute = 0;
        std::uint64_t endMinute = 0;
        std::uint32_t durationMinutes = 0;
        std::uint32_t tickMinutes = 1;
        std::uint32_t iterationCount = 0;
        std::uint32_t finalTickMinutes = 0;
        std::uint32_t multiplier = 1;
        bool clamped = false;
        bool valid = false;
    };

    class SimulationRunPlanner
    {
    public:
        static SimulationRunPlan Build(SimulationRunRequest const& request);
    };
}

#endif
