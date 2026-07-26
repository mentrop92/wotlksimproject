#ifndef MOD_LIVINGWORLD_SIMULATION_RUN_EVENTS_H
#define MOD_LIVINGWORLD_SIMULATION_RUN_EVENTS_H

#include "LivingWorldSimulationRunState.h"

#include <cstdint>
#include <deque>
#include <string>

namespace LivingWorld
{
    namespace SimulationRunEventLimits
    {
        inline constexpr std::size_t MaximumEvents = 128;
        inline constexpr std::size_t MaximumDetailLength = 96;
    }

    enum class SimulationRunEventType : std::uint8_t
    {
        Created = 0,
        Started,
        Progress,
        Completed,
        Cancelled,
        Failed
    };

    struct SimulationRunEvent
    {
        SimulationRunEventType type = SimulationRunEventType::Created;
        std::uint64_t simulationMinute = 0;
        std::uint32_t progressBasisPoints = 0;
        SimulationStopReason stopReason = SimulationStopReason::None;
        std::string detail;
        bool clamped = false;
    };

    class SimulationRunEventHistory
    {
    public:
        bool Append(SimulationRunEvent event);
        bool AppendSnapshot(SimulationRunSnapshot const& snapshot, std::string detail = {});
        void Reset();

        std::deque<SimulationRunEvent> const& Events() const;
        std::size_t Size() const;
        bool Empty() const;

    private:
        static SimulationRunEventType TypeForSnapshot(SimulationRunSnapshot const& snapshot);
        static std::string SanitizeDetail(std::string detail, bool& clamped);

        std::deque<SimulationRunEvent> _events;
    };
}

#endif
