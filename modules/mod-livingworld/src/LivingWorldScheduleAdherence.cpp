#include "LivingWorldScheduleAdherence.h"

#include <algorithm>

namespace LivingWorld
{
    ScheduleAdherenceDecision ScheduleAdherence::Evaluate(ScheduleAdherenceState const& state)
    {
        ScheduleAdherenceDecision decision;
        if (state.recentPlannedSessions > 0)
        {
            std::uint32_t const attended = std::min<std::uint32_t>(state.recentAttendedSessions, state.recentPlannedSessions);
            decision.attendancePercent = static_cast<std::uint8_t>((attended * 100U) / state.recentPlannedSessions);
        }

        decision.weeklyTargetReached = state.targetMinutesThisWeek > 0 &&
            state.minutesPlayedThisWeek >= state.targetMinutesThisWeek;

        if (decision.weeklyTargetReached)
        {
            decision.loginPriorityAdjustment = -200;
            return decision;
        }

        std::uint32_t const remainingMinutes = state.targetMinutesThisWeek > state.minutesPlayedThisWeek
            ? state.targetMinutesThisWeek - state.minutesPlayedThisWeek
            : 0;

        decision.needsCatchUpSession = state.consecutiveMisses >= 2 ||
            (state.targetMinutesThisWeek > 0 && remainingMinutes * 4U >= state.targetMinutesThisWeek);

        std::int32_t adjustment = static_cast<std::int32_t>(state.consecutiveMisses) * 75;
        if (decision.needsCatchUpSession)
            adjustment += 150;
        if (decision.attendancePercent < 50)
            adjustment += 100;

        decision.loginPriorityAdjustment = static_cast<std::int16_t>(std::clamp<std::int32_t>(adjustment, -500, 500));
        return decision;
    }
}
