#include "LivingWorldScheduleAdherence.h"

#include <cassert>
#include <iostream>

using namespace LivingWorld;

int main()
{
    ScheduleAdherenceDecision empty = ScheduleAdherence::Evaluate({});
    assert(empty.attendancePercent == 100);
    assert(empty.loginPriorityAdjustment == 0);
    assert(!empty.needsCatchUpSession);

    ScheduleAdherenceState missed;
    missed.recentPlannedSessions = 4;
    missed.recentAttendedSessions = 1;
    missed.consecutiveMisses = 3;
    missed.minutesPlayedThisWeek = 120;
    missed.targetMinutesThisWeek = 600;
    ScheduleAdherenceDecision catchUp = ScheduleAdherence::Evaluate(missed);
    assert(catchUp.attendancePercent == 25);
    assert(catchUp.needsCatchUpSession);
    assert(catchUp.loginPriorityAdjustment == 475);
    assert(!catchUp.weeklyTargetReached);

    ScheduleAdherenceState reached;
    reached.recentPlannedSessions = 5;
    reached.recentAttendedSessions = 5;
    reached.minutesPlayedThisWeek = 700;
    reached.targetMinutesThisWeek = 600;
    ScheduleAdherenceDecision complete = ScheduleAdherence::Evaluate(reached);
    assert(complete.attendancePercent == 100);
    assert(complete.weeklyTargetReached);
    assert(complete.loginPriorityAdjustment == -200);
    assert(!complete.needsCatchUpSession);

    ScheduleAdherenceState bounded;
    bounded.recentPlannedSessions = 10;
    bounded.recentAttendedSessions = 0;
    bounded.consecutiveMisses = 20;
    bounded.targetMinutesThisWeek = 1000;
    ScheduleAdherenceDecision capped = ScheduleAdherence::Evaluate(bounded);
    assert(capped.loginPriorityAdjustment == 500);

    std::cout << "LivingWorld schedule adherence tests passed.\n";
    return 0;
}
