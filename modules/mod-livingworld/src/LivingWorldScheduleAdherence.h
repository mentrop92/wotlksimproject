#ifndef MOD_LIVINGWORLD_SCHEDULE_ADHERENCE_H
#define MOD_LIVINGWORLD_SCHEDULE_ADHERENCE_H

#include <cstdint>

namespace LivingWorld
{
    struct ScheduleAdherenceState
    {
        std::uint8_t recentPlannedSessions = 0;
        std::uint8_t recentAttendedSessions = 0;
        std::uint8_t consecutiveMisses = 0;
        std::uint16_t minutesPlayedThisWeek = 0;
        std::uint16_t targetMinutesThisWeek = 0;
    };

    struct ScheduleAdherenceDecision
    {
        std::uint8_t attendancePercent = 100;
        std::int16_t loginPriorityAdjustment = 0;
        bool needsCatchUpSession = false;
        bool weeklyTargetReached = false;
    };

    class ScheduleAdherence
    {
    public:
        static ScheduleAdherenceDecision Evaluate(ScheduleAdherenceState const& state);
    };
}

#endif
