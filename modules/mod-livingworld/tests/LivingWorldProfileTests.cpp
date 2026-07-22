#include "LivingWorldProfile.h"

#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>

namespace
{
    bool Equal(LivingWorld::BotProfile const& left, LivingWorld::BotProfile const& right)
    {
        return left.characterGuid == right.characterGuid &&
            left.seed == right.seed &&
            left.schemaVersion == right.schemaVersion &&
            left.primaryGoal == right.primaryGoal &&
            left.personality.ambition == right.personality.ambition &&
            left.personality.sociability == right.personality.sociability &&
            left.personality.competitiveness == right.personality.competitiveness &&
            left.personality.curiosity == right.personality.curiosity &&
            left.personality.loyalty == right.personality.loyalty &&
            left.personality.patience == right.personality.patience &&
            left.personality.riskTolerance == right.personality.riskTolerance &&
            left.intelligence.tactical == right.intelligence.tactical &&
            left.intelligence.strategic == right.intelligence.strategic &&
            left.intelligence.social == right.intelligence.social &&
            left.intelligence.economic == right.intelligence.economic &&
            left.intelligence.gameKnowledge == right.intelligence.gameKnowledge &&
            left.intelligence.adaptability == right.intelligence.adaptability &&
            left.intelligence.attention == right.intelligence.attention &&
            left.lifestyle.archetype == right.lifestyle.archetype &&
            left.lifestyle.targetWeeklyHours == right.lifestyle.targetWeeklyHours &&
            left.lifestyle.minimumSessionMinutes == right.lifestyle.minimumSessionMinutes &&
            left.lifestyle.maximumSessionMinutes == right.lifestyle.maximumSessionMinutes &&
            left.lifestyle.scheduleConsistency == right.lifestyle.scheduleConsistency &&
            left.lifestyle.skipSessionChance == right.lifestyle.skipSessionChance &&
            left.lifestyle.marathonSessionChance == right.lifestyle.marathonSessionChance &&
            left.lifestyle.burnoutSensitivity == right.lifestyle.burnoutSensitivity &&
            left.preferences.leveling == right.preferences.leveling &&
            left.preferences.questing == right.preferences.questing &&
            left.preferences.dungeons == right.preferences.dungeons &&
            left.preferences.raiding == right.preferences.raiding &&
            left.preferences.pvp == right.preferences.pvp &&
            left.preferences.professions == right.preferences.professions &&
            left.preferences.collecting == right.preferences.collecting &&
            left.preferences.socializing == right.preferences.socializing;
    }

    bool ValidScore(std::uint8_t value)
    {
        return value >= 1 && value <= 100;
    }

    void ValidateScores(LivingWorld::BotProfile const& profile)
    {
        assert(ValidScore(profile.personality.ambition));
        assert(ValidScore(profile.personality.sociability));
        assert(ValidScore(profile.personality.competitiveness));
        assert(ValidScore(profile.personality.curiosity));
        assert(ValidScore(profile.personality.loyalty));
        assert(ValidScore(profile.personality.patience));
        assert(ValidScore(profile.personality.riskTolerance));

        assert(ValidScore(profile.intelligence.tactical));
        assert(ValidScore(profile.intelligence.strategic));
        assert(ValidScore(profile.intelligence.social));
        assert(ValidScore(profile.intelligence.economic));
        assert(ValidScore(profile.intelligence.gameKnowledge));
        assert(ValidScore(profile.intelligence.adaptability));
        assert(ValidScore(profile.intelligence.attention));

        assert(ValidScore(profile.lifestyle.scheduleConsistency));
        assert(profile.lifestyle.skipSessionChance <= 100);
        assert(profile.lifestyle.marathonSessionChance <= 100);
        assert(ValidScore(profile.lifestyle.burnoutSensitivity));

        assert(ValidScore(profile.preferences.leveling));
        assert(ValidScore(profile.preferences.questing));
        assert(ValidScore(profile.preferences.dungeons));
        assert(ValidScore(profile.preferences.raiding));
        assert(ValidScore(profile.preferences.pvp));
        assert(ValidScore(profile.preferences.professions));
        assert(ValidScore(profile.preferences.collecting));
        assert(ValidScore(profile.preferences.socializing));
    }
}

int main()
{
    using namespace LivingWorld;

    BotProfile const fixtureA = ProfileGenerator::Generate(42, 1, 1);
    BotProfile const fixtureB = ProfileGenerator::Generate(42, 1, 1);
    BotProfile const different = ProfileGenerator::Generate(43, 1, 1);

    assert(Equal(fixtureA, fixtureB));
    assert(fixtureA.seed != different.seed);

    constexpr std::uint32_t populationSize = 10000;
    std::array<std::uint32_t, 7> archetypeCounts{};

    for (std::uint32_t guid = 1; guid <= populationSize; ++guid)
    {
        std::uint8_t const race = static_cast<std::uint8_t>(1 + (guid % 10));
        std::uint8_t const playerClass = static_cast<std::uint8_t>(1 + (guid % 11));
        BotProfile const profile = ProfileGenerator::Generate(guid, race, playerClass);
        ValidateScores(profile);
        ++archetypeCounts[static_cast<std::size_t>(profile.lifestyle.archetype)];
    }

    auto inRange = [&](ActivityArchetype archetype, std::uint32_t minimum, std::uint32_t maximum)
    {
        std::uint32_t const count = archetypeCounts[static_cast<std::size_t>(archetype)];
        return count >= minimum && count <= maximum;
    };

    assert(inRange(ActivityArchetype::NoLifeGrinder, 50, 350));
    assert(inRange(ActivityArchetype::Hardcore, 400, 1100));
    assert(inRange(ActivityArchetype::Dedicated, 900, 1700));
    assert(inRange(ActivityArchetype::Regular, 3100, 4100));
    assert(inRange(ActivityArchetype::Weekend, 1400, 2200));
    assert(inRange(ActivityArchetype::Casual, 1500, 2300));
    assert(inRange(ActivityArchetype::Dormant, 300, 800));

    std::cout << "LivingWorld profile tests passed for " << populationSize << " generated characters.\n";
    return 0;
}
