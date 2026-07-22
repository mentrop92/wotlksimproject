#include "LivingWorldProfile.h"

#include <algorithm>

namespace LivingWorld
{
    namespace
    {
        // SplitMix64 gives us a small, deterministic generator suitable for
        // deriving stable profile traits. It is not used for security.
        std::uint64_t Next(std::uint64_t& state)
        {
            std::uint64_t value = (state += 0x9E3779B97F4A7C15ULL);
            value = (value ^ (value >> 30U)) * 0xBF58476D1CE4E5B9ULL;
            value = (value ^ (value >> 27U)) * 0x94D049BB133111EBULL;
            return value ^ (value >> 31U);
        }

        std::uint8_t Score(std::uint64_t& state, std::uint8_t minimum = 1, std::uint8_t maximum = 100)
        {
            const std::uint64_t width = static_cast<std::uint64_t>(maximum - minimum) + 1ULL;
            return static_cast<std::uint8_t>(minimum + (Next(state) % width));
        }

        std::uint16_t Range(std::uint64_t& state, std::uint16_t minimum, std::uint16_t maximum)
        {
            const std::uint64_t width = static_cast<std::uint64_t>(maximum - minimum) + 1ULL;
            return static_cast<std::uint16_t>(minimum + (Next(state) % width));
        }

        std::uint8_t ClampScore(int value)
        {
            return static_cast<std::uint8_t>(std::clamp(value, 1, 100));
        }

        ActivityArchetype GenerateLifestyle(std::uint64_t& state, LifestyleTraits& lifestyle)
        {
            const std::uint8_t roll = Score(state, 0, 99);

            if (roll <= 1)
            {
                lifestyle.targetWeeklyHours = Range(state, 140, 168);
                lifestyle.minimumSessionMinutes = Range(state, 360, 600);
                lifestyle.maximumSessionMinutes = Range(state, 900, 1440);
                lifestyle.skipSessionChance = Score(state, 0, 3);
                lifestyle.marathonSessionChance = Score(state, 70, 100);
                return ActivityArchetype::NoLifeGrinder;
            }

            if (roll <= 8)
            {
                lifestyle.targetWeeklyHours = Range(state, 65, 110);
                lifestyle.minimumSessionMinutes = Range(state, 180, 360);
                lifestyle.maximumSessionMinutes = Range(state, 480, 900);
                lifestyle.skipSessionChance = Score(state, 2, 10);
                lifestyle.marathonSessionChance = Score(state, 35, 75);
                return ActivityArchetype::Hardcore;
            }

            if (roll <= 21)
            {
                lifestyle.targetWeeklyHours = Range(state, 38, 65);
                lifestyle.minimumSessionMinutes = Range(state, 120, 240);
                lifestyle.maximumSessionMinutes = Range(state, 300, 600);
                lifestyle.skipSessionChance = Score(state, 5, 15);
                lifestyle.marathonSessionChance = Score(state, 15, 45);
                return ActivityArchetype::Dedicated;
            }

            if (roll <= 57)
            {
                lifestyle.targetWeeklyHours = Range(state, 16, 38);
                lifestyle.minimumSessionMinutes = Range(state, 60, 150);
                lifestyle.maximumSessionMinutes = Range(state, 180, 360);
                lifestyle.skipSessionChance = Score(state, 10, 30);
                lifestyle.marathonSessionChance = Score(state, 5, 20);
                return ActivityArchetype::Regular;
            }

            if (roll <= 75)
            {
                lifestyle.targetWeeklyHours = Range(state, 8, 20);
                lifestyle.minimumSessionMinutes = Range(state, 60, 180);
                lifestyle.maximumSessionMinutes = Range(state, 240, 540);
                lifestyle.skipSessionChance = Score(state, 15, 40);
                lifestyle.marathonSessionChance = Score(state, 10, 35);
                return ActivityArchetype::Weekend;
            }

            if (roll <= 94)
            {
                lifestyle.targetWeeklyHours = Range(state, 3, 12);
                lifestyle.minimumSessionMinutes = Range(state, 30, 90);
                lifestyle.maximumSessionMinutes = Range(state, 90, 240);
                lifestyle.skipSessionChance = Score(state, 25, 60);
                lifestyle.marathonSessionChance = Score(state, 1, 10);
                return ActivityArchetype::Casual;
            }

            lifestyle.targetWeeklyHours = Range(state, 1, 5);
            lifestyle.minimumSessionMinutes = Range(state, 20, 60);
            lifestyle.maximumSessionMinutes = Range(state, 60, 120);
            lifestyle.skipSessionChance = Score(state, 60, 90);
            lifestyle.marathonSessionChance = Score(state, 0, 3);
            return ActivityArchetype::Dormant;
        }
    }

    BotProfile ProfileGenerator::Generate(std::uint32_t characterGuid, std::uint8_t raceId, std::uint8_t classId)
    {
        BotProfile profile;
        profile.characterGuid = characterGuid;

        // Mix identity inputs with a fixed generator-version salt. Changing the
        // salt requires a profile schema migration and should never happen by accident.
        std::uint64_t state = 0x4C4956494E475731ULL;
        state ^= static_cast<std::uint64_t>(characterGuid) * 0x9E3779B97F4A7C15ULL;
        state ^= static_cast<std::uint64_t>(raceId) << 40U;
        state ^= static_cast<std::uint64_t>(classId) << 48U;
        profile.seed = Next(state);
        state = profile.seed;

        profile.personality.ambition = Score(state);
        profile.personality.sociability = Score(state);
        profile.personality.competitiveness = Score(state);
        profile.personality.curiosity = Score(state);
        profile.personality.loyalty = Score(state);
        profile.personality.patience = Score(state);
        profile.personality.riskTolerance = Score(state);

        // Intelligence dimensions share a loose baseline but retain enough
        // variation for specialists, average players, and broadly gifted bots.
        const int intelligenceBaseline = Score(state, 25, 85);
        profile.intelligence.tactical = ClampScore(intelligenceBaseline + static_cast<int>(Score(state, 0, 40)) - 20);
        profile.intelligence.strategic = ClampScore(intelligenceBaseline + static_cast<int>(Score(state, 0, 40)) - 20);
        profile.intelligence.social = ClampScore(intelligenceBaseline + static_cast<int>(Score(state, 0, 50)) - 25);
        profile.intelligence.economic = ClampScore(intelligenceBaseline + static_cast<int>(Score(state, 0, 50)) - 25);
        profile.intelligence.gameKnowledge = ClampScore(intelligenceBaseline + static_cast<int>(Score(state, 0, 45)) - 22);
        profile.intelligence.adaptability = ClampScore(intelligenceBaseline + static_cast<int>(Score(state, 0, 45)) - 22);
        profile.intelligence.attention = ClampScore(intelligenceBaseline + static_cast<int>(Score(state, 0, 55)) - 27);

        profile.lifestyle.archetype = GenerateLifestyle(state, profile.lifestyle);
        profile.lifestyle.scheduleConsistency = Score(state);
        profile.lifestyle.burnoutSensitivity = Score(state);

        profile.preferences.leveling = Score(state);
        profile.preferences.questing = Score(state);
        profile.preferences.dungeons = Score(state);
        profile.preferences.raiding = Score(state);
        profile.preferences.pvp = Score(state);
        profile.preferences.professions = Score(state);
        profile.preferences.collecting = Score(state);
        profile.preferences.socializing = Score(state);

        // Ambition and activity level should influence leveling interest without
        // making every highly active bot identical.
        const int activityBonus = profile.lifestyle.targetWeeklyHours >= 65 ? 15 :
            (profile.lifestyle.targetWeeklyHours >= 38 ? 8 : 0);
        profile.preferences.leveling = ClampScore(
            (static_cast<int>(profile.preferences.leveling) +
             static_cast<int>(profile.personality.ambition)) / 2 + activityBonus);

        const std::uint8_t goalRoll = Score(state, 0, 99);
        if (goalRoll < profile.preferences.leveling / 4)
            profile.primaryGoal = GoalArchetype::Leveling;
        else if (goalRoll < 35)
            profile.primaryGoal = GoalArchetype::Dungeons;
        else if (goalRoll < 50)
            profile.primaryGoal = GoalArchetype::Raiding;
        else if (goalRoll < 65)
            profile.primaryGoal = GoalArchetype::PvP;
        else if (goalRoll < 76)
            profile.primaryGoal = GoalArchetype::Economy;
        else if (goalRoll < 86)
            profile.primaryGoal = GoalArchetype::Professions;
        else if (goalRoll < 94)
            profile.primaryGoal = GoalArchetype::Collecting;
        else
            profile.primaryGoal = GoalArchetype::SocialLeadership;

        return profile;
    }
}
