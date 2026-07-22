/*
 * LivingWorld persistent identity model.
 *
 * The profile is intentionally independent from Playerbots combat code. The
 * LivingWorld layer decides who a bot is, how often it plays, and what it
 * prefers; Playerbots remains responsible for moment-to-moment gameplay.
 */

#ifndef MOD_LIVINGWORLD_PROFILE_H
#define MOD_LIVINGWORLD_PROFILE_H

#include <cstdint>

namespace LivingWorld
{
    constexpr std::uint16_t PROFILE_SCHEMA_VERSION = 1;

    enum class ActivityArchetype : std::uint8_t
    {
        Dormant = 0,
        Casual,
        Weekend,
        Regular,
        Dedicated,
        Hardcore,
        NoLifeGrinder
    };

    enum class GoalArchetype : std::uint8_t
    {
        Leveling = 0,
        Dungeons,
        Raiding,
        PvP,
        Economy,
        Professions,
        Collecting,
        SocialLeadership
    };

    struct PersonalityTraits
    {
        std::uint8_t ambition = 50;
        std::uint8_t sociability = 50;
        std::uint8_t competitiveness = 50;
        std::uint8_t curiosity = 50;
        std::uint8_t loyalty = 50;
        std::uint8_t patience = 50;
        std::uint8_t riskTolerance = 50;
    };

    struct IntelligenceTraits
    {
        std::uint8_t tactical = 50;
        std::uint8_t strategic = 50;
        std::uint8_t social = 50;
        std::uint8_t economic = 50;
        std::uint8_t gameKnowledge = 50;
        std::uint8_t adaptability = 50;
        std::uint8_t attention = 50;
    };

    struct LifestyleTraits
    {
        ActivityArchetype archetype = ActivityArchetype::Regular;
        std::uint16_t targetWeeklyHours = 20;
        std::uint16_t minimumSessionMinutes = 45;
        std::uint16_t maximumSessionMinutes = 180;
        std::uint8_t scheduleConsistency = 50;
        std::uint8_t skipSessionChance = 20;
        std::uint8_t marathonSessionChance = 5;
        std::uint8_t burnoutSensitivity = 50;
    };

    struct ActivityPreferences
    {
        std::uint8_t leveling = 50;
        std::uint8_t questing = 50;
        std::uint8_t dungeons = 50;
        std::uint8_t raiding = 50;
        std::uint8_t pvp = 50;
        std::uint8_t professions = 50;
        std::uint8_t collecting = 50;
        std::uint8_t socializing = 50;
    };

    struct BotProfile
    {
        std::uint32_t characterGuid = 0;
        std::uint64_t seed = 0;
        std::uint16_t schemaVersion = PROFILE_SCHEMA_VERSION;
        GoalArchetype primaryGoal = GoalArchetype::Leveling;
        PersonalityTraits personality;
        IntelligenceTraits intelligence;
        LifestyleTraits lifestyle;
        ActivityPreferences preferences;
    };

    class ProfileGenerator
    {
    public:
        // Generation is deterministic. The same character GUID, race, class,
        // and generator version always produce the same initial identity.
        static BotProfile Generate(std::uint32_t characterGuid, std::uint8_t raceId, std::uint8_t classId);
    };
}

#endif
