#include "LivingWorldSimulationRunPlan.h"

#include <algorithm>
#include <limits>

namespace LivingWorld
{
    SimulationRunPlan SimulationRunPlanner::Build(SimulationRunRequest const& request)
    {
        SimulationRunPlan plan;
        plan.startMinute = std::min(request.startMinute, SimulationClockLimits::MaximumSimulationMinute);
        plan.durationMinutes = std::min(request.durationMinutes, SimulationRunLimits::MaximumRequestedMinutes);
        plan.tickMinutes = std::clamp<std::uint32_t>(request.tickMinutes, 1, SimulationRunLimits::MaximumTickMinutes);
        plan.multiplier = std::clamp<std::uint32_t>(request.multiplier, 1, SimulationClockLimits::MaximumMultiplier);
        plan.clamped = plan.startMinute != request.startMinute ||
            plan.durationMinutes != request.durationMinutes ||
            plan.tickMinutes != request.tickMinutes ||
            plan.multiplier != request.multiplier;

        if (plan.durationMinutes == 0 || plan.startMinute >= SimulationClockLimits::MaximumSimulationMinute)
            return plan;

        std::uint32_t minimumTick = static_cast<std::uint32_t>(
            (static_cast<std::uint64_t>(plan.durationMinutes) + SimulationRunLimits::MaximumIterations - 1) /
            SimulationRunLimits::MaximumIterations);
        minimumTick = std::max<std::uint32_t>(minimumTick, 1);
        if (plan.tickMinutes < minimumTick)
        {
            plan.tickMinutes = minimumTick;
            plan.clamped = true;
        }

        std::uint64_t maximumDuration = SimulationClockLimits::MaximumSimulationMinute - plan.startMinute;
        if (plan.durationMinutes > maximumDuration)
        {
            plan.durationMinutes = static_cast<std::uint32_t>(maximumDuration);
            plan.clamped = true;
        }

        if (plan.durationMinutes == 0)
            return plan;

        plan.iterationCount = static_cast<std::uint32_t>(
            (static_cast<std::uint64_t>(plan.durationMinutes) + plan.tickMinutes - 1) / plan.tickMinutes);
        plan.finalTickMinutes = plan.durationMinutes % plan.tickMinutes;
        if (plan.finalTickMinutes == 0)
            plan.finalTickMinutes = plan.tickMinutes;
        plan.endMinute = plan.startMinute + plan.durationMinutes;
        plan.valid = plan.iterationCount > 0 && plan.iterationCount <= SimulationRunLimits::MaximumIterations;
        return plan;
    }
}
