#include "LivingWorldSchedulePlanner.h"

#include <cassert>
#include <iostream>

using namespace LivingWorld;

int main()
{
    BotProfile profile;
    profile.seed = 123456789ULL;
    profile.lifestyle.archetype = ActivityArchetype::Weekend;
    profile.lifestyle.minimumSessionMinutes = 60;
    profile.lifestyle.maximumSessionMinutes = 180;
    profile.lifestyle.skipSessionChance = 25;
    profile.lifestyle.marathonSessionChance = 0;

    WeeklySchedulePlan const first = SchedulePlanner::Build(profile);
    WeeklySchedulePlan const second = SchedulePlanner::Build(profile);

    assert(first.dayMask == 0b1100000);
    assert(first.dayMask == second.dayMask);
    assert(first.preferredStartMinute == second.preferredStartMinute);
    assert(first.plannedSessionMinutes == second.plannedSessionMinutes);
    assert(first.skipSessionChance == 25);
    assert(first.plannedSessionMinutes >= 60);
    assert(first.plannedSessionMinutes <= 180);
    assert(!first.marathonEligible);

    profile.seed = 987654321ULL;
    profile.lifestyle.archetype = ActivityArchetype::NoLifeGrinder;
    profile.lifestyle.minimumSessionMinutes = 240;
    profile.lifestyle.maximumSessionMinutes = 480;
    profile.lifestyle.marathonSessionChance = 100;

    WeeklySchedulePlan const grinder = SchedulePlanner::Build(profile);
    assert(grinder.dayMask == 0b1111111);
    assert(grinder.marathonEligible);
    assert(grinder.plannedSessionMinutes >= 420);
    assert(grinder.plannedSessionMinutes <= 720);

    std::cout << "LivingWorld schedule planner tests passed.\n";
    return 0;
}
