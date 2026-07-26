#include "LivingWorldSimulationRunEvents.h"

#include <algorithm>

namespace LivingWorld
{
    bool SimulationRunEventHistory::Append(SimulationRunEvent event)
    {
        if (!_events.empty() && event.simulationMinute < _events.back().simulationMinute)
            return false;

        event.progressBasisPoints = std::min<std::uint32_t>(event.progressBasisPoints, 10000);
        event.detail = SanitizeDetail(std::move(event.detail), event.clamped);

        _events.push_back(std::move(event));
        while (_events.size() > SimulationRunEventLimits::MaximumEvents)
            _events.pop_front();

        return true;
    }

    bool SimulationRunEventHistory::AppendSnapshot(SimulationRunSnapshot const& snapshot, std::string detail)
    {
        SimulationRunEvent event;
        event.type = TypeForSnapshot(snapshot);
        event.simulationMinute = snapshot.currentMinute;
        event.progressBasisPoints = snapshot.progressBasisPoints;
        event.stopReason = snapshot.stopReason;
        event.detail = std::move(detail);
        event.clamped = snapshot.clamped;
        return Append(std::move(event));
    }

    void SimulationRunEventHistory::Reset()
    {
        _events.clear();
    }

    std::deque<SimulationRunEvent> const& SimulationRunEventHistory::Events() const
    {
        return _events;
    }

    std::size_t SimulationRunEventHistory::Size() const
    {
        return _events.size();
    }

    bool SimulationRunEventHistory::Empty() const
    {
        return _events.empty();
    }

    SimulationRunEventType SimulationRunEventHistory::TypeForSnapshot(SimulationRunSnapshot const& snapshot)
    {
        switch (snapshot.status)
        {
            case SimulationRunStatus::Pending: return SimulationRunEventType::Created;
            case SimulationRunStatus::Running:
                return snapshot.completedIterations == 0
                    ? SimulationRunEventType::Started
                    : SimulationRunEventType::Progress;
            case SimulationRunStatus::Completed: return SimulationRunEventType::Completed;
            case SimulationRunStatus::Cancelled: return SimulationRunEventType::Cancelled;
            case SimulationRunStatus::Failed: return SimulationRunEventType::Failed;
        }

        return SimulationRunEventType::Failed;
    }

    std::string SimulationRunEventHistory::SanitizeDetail(std::string detail, bool& clamped)
    {
        for (char& character : detail)
        {
            unsigned char const value = static_cast<unsigned char>(character);
            if (value < 0x20 || value == 0x7f)
            {
                character = ' ';
                clamped = true;
            }
        }

        if (detail.size() > SimulationRunEventLimits::MaximumDetailLength)
        {
            detail.resize(SimulationRunEventLimits::MaximumDetailLength);
            clamped = true;
        }

        return detail;
    }
}
