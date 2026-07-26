#include "LivingWorldSimulationRunState.h"

#include <algorithm>

namespace LivingWorld
{
    SimulationRunState SimulationRunState::Create(SimulationRunPlan const& plan)
    {
        SimulationRunState state;
        state._snapshot.startMinute = plan.startMinute;
        state._snapshot.currentMinute = plan.startMinute;
        state._snapshot.endMinute = plan.endMinute;
        state._snapshot.plannedIterations = std::min(plan.iterationCount, SimulationRunLimits::MaximumIterations);
        state._snapshot.clamped = plan.clamped || plan.iterationCount > SimulationRunLimits::MaximumIterations;

        if (!plan.valid || plan.endMinute <= plan.startMinute || state._snapshot.plannedIterations == 0)
        {
            state._snapshot.status = SimulationRunStatus::Failed;
            state._snapshot.stopReason = SimulationStopReason::InvalidPlan;
            state._snapshot.progressBasisPoints = 0;
        }

        return state;
    }

    bool SimulationRunState::Start()
    {
        if (_snapshot.status != SimulationRunStatus::Pending)
            return false;

        _snapshot.status = SimulationRunStatus::Running;
        return true;
    }

    bool SimulationRunState::Advance(std::uint32_t simulatedMinutes)
    {
        if (_snapshot.status != SimulationRunStatus::Running || simulatedMinutes == 0)
            return false;

        std::uint64_t const remaining = _snapshot.endMinute - _snapshot.currentMinute;
        std::uint64_t const applied = std::min<std::uint64_t>(simulatedMinutes, remaining);
        _snapshot.currentMinute += applied;

        if (_snapshot.completedIterations < _snapshot.plannedIterations)
            ++_snapshot.completedIterations;
        else
            _snapshot.clamped = true;

        RefreshProgress();
        CompleteIfFinished();
        return applied > 0;
    }

    bool SimulationRunState::Cancel(SimulationStopReason reason)
    {
        if (IsTerminal())
            return false;

        _snapshot.status = SimulationRunStatus::Cancelled;
        _snapshot.stopReason = reason == SimulationStopReason::None ? SimulationStopReason::UserRequested : reason;
        RefreshProgress();
        return true;
    }

    bool SimulationRunState::Fail(SimulationStopReason reason)
    {
        if (IsTerminal())
            return false;

        _snapshot.status = SimulationRunStatus::Failed;
        _snapshot.stopReason = reason == SimulationStopReason::None ? SimulationStopReason::SafetyLimit : reason;
        RefreshProgress();
        return true;
    }

    SimulationRunSnapshot const& SimulationRunState::GetSnapshot() const
    {
        return _snapshot;
    }

    bool SimulationRunState::IsTerminal() const
    {
        return _snapshot.status == SimulationRunStatus::Completed ||
            _snapshot.status == SimulationRunStatus::Cancelled ||
            _snapshot.status == SimulationRunStatus::Failed;
    }

    void SimulationRunState::RefreshProgress()
    {
        if (_snapshot.endMinute <= _snapshot.startMinute)
        {
            _snapshot.progressBasisPoints = 0;
            return;
        }

        std::uint64_t const elapsed = _snapshot.currentMinute - _snapshot.startMinute;
        std::uint64_t const duration = _snapshot.endMinute - _snapshot.startMinute;
        _snapshot.progressBasisPoints = static_cast<std::uint32_t>(
            std::min<std::uint64_t>(10000ULL, (elapsed * 10000ULL) / duration));
    }

    void SimulationRunState::CompleteIfFinished()
    {
        if (_snapshot.currentMinute < _snapshot.endMinute)
            return;

        _snapshot.currentMinute = _snapshot.endMinute;
        _snapshot.completedIterations = _snapshot.plannedIterations;
        _snapshot.progressBasisPoints = 10000;
        _snapshot.status = SimulationRunStatus::Completed;
        _snapshot.stopReason = SimulationStopReason::None;
    }
}
