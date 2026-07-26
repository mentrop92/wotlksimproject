#ifndef MOD_LIVINGWORLD_SIMULATION_RUN_STATE_H
#define MOD_LIVINGWORLD_SIMULATION_RUN_STATE_H

#include "LivingWorldSimulationRunPlan.h"

#include <cstdint>

namespace LivingWorld
{
    enum class SimulationRunStatus : std::uint8_t
    {
        Pending = 0,
        Running,
        Completed,
        Cancelled,
        Failed
    };

    enum class SimulationStopReason : std::uint8_t
    {
        None = 0,
        UserRequested,
        SafetyLimit,
        InvalidPlan,
        TimelineExhausted
    };

    struct SimulationRunSnapshot
    {
        SimulationRunStatus status = SimulationRunStatus::Pending;
        SimulationStopReason stopReason = SimulationStopReason::None;
        std::uint64_t startMinute = 0;
        std::uint64_t currentMinute = 0;
        std::uint64_t endMinute = 0;
        std::uint32_t plannedIterations = 0;
        std::uint32_t completedIterations = 0;
        std::uint32_t progressBasisPoints = 0;
        bool clamped = false;
    };

    class SimulationRunState
    {
    public:
        static SimulationRunState Create(SimulationRunPlan const& plan);

        bool Start();
        bool Advance(std::uint32_t simulatedMinutes);
        bool Cancel(SimulationStopReason reason = SimulationStopReason::UserRequested);
        bool Fail(SimulationStopReason reason);

        SimulationRunSnapshot const& GetSnapshot() const;
        bool IsTerminal() const;

    private:
        void RefreshProgress();
        void CompleteIfFinished();

        SimulationRunSnapshot _snapshot;
    };
}

#endif
