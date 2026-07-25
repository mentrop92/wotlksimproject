#include "LivingWorldProfileManager.h"

#include "DatabaseEnv.h"
#include "Log.h"
#include "Player.h"

namespace LivingWorld
{
    ProfileManager& ProfileManager::Instance()
    {
        static ProfileManager instance;
        return instance;
    }

    BotProfile const* ProfileManager::EnsureProfile(Player* player)
    {
        if (!player)
            return nullptr;

        std::uint32_t const characterGuid = player->GetGUID().GetCounter();
        if (auto const itr = _profiles.find(characterGuid); itr != _profiles.end())
            return &itr->second;

        if (std::optional<BotProfile> stored = LoadFromDatabase(characterGuid))
        {
            auto const [itr, inserted] = _profiles.emplace(characterGuid, *stored);
            (void)inserted;
            return &itr->second;
        }

        BotProfile generated = ProfileGenerator::Generate(characterGuid, player->getRace(), player->getClass());
        if (!InsertProfile(generated))
        {
            LOG_ERROR("module.livingworld", "Failed to persist LivingWorld profile for character GUID {}.", characterGuid);
            return nullptr;
        }

        auto const [itr, inserted] = _profiles.emplace(characterGuid, generated);
        if (!inserted)
            return &itr->second;

        LOG_INFO(
            "module.livingworld",
            "Created LivingWorld profile for {} (GUID {}, schema {}, seed {}).",
            player->GetName(),
            characterGuid,
            generated.schemaVersion,
            generated.seed);

        return &itr->second;
    }

    BotProfile const* ProfileManager::GetProfile(std::uint32_t characterGuid) const
    {
        if (auto const itr = _profiles.find(characterGuid); itr != _profiles.end())
            return &itr->second;

        return nullptr;
    }

    bool ProfileManager::IsLoaded(std::uint32_t characterGuid) const
    {
        return _profiles.find(characterGuid) != _profiles.end();
    }

    void ProfileManager::UnloadProfile(std::uint32_t characterGuid, bool updateLastSeen)
    {
        auto const itr = _profiles.find(characterGuid);
        if (itr == _profiles.end())
            return;

        if (updateLastSeen)
        {
            CharacterDatabase.Execute(
                "UPDATE `lw_profiles` SET `last_seen_at` = CURRENT_TIMESTAMP WHERE `character_guid` = {}",
                characterGuid);
        }

        _profiles.erase(itr);
    }

    std::size_t ProfileManager::LoadedCount() const
    {
        return _profiles.size();
    }

    std::optional<BotProfile> ProfileManager::LoadFromDatabase(std::uint32_t characterGuid) const
    {
        QueryResult result = CharacterDatabase.Query(
            "SELECT "
            "`seed`, `schema_version`, `primary_goal`, `activity_archetype`, "
            "`ambition`, `sociability`, `competitiveness`, `curiosity`, `loyalty`, `patience`, `risk_tolerance`, "
            "`tactical_intelligence`, `strategic_intelligence`, `social_intelligence`, `economic_intelligence`, "
            "`game_knowledge`, `adaptability`, `attention`, "
            "`target_weekly_hours`, `minimum_session_minutes`, `maximum_session_minutes`, "
            "`schedule_consistency`, `skip_session_chance`, `marathon_session_chance`, `burnout_sensitivity`, "
            "`leveling_preference`, `questing_preference`, `dungeon_preference`, `raid_preference`, "
            "`pvp_preference`, `profession_preference`, `collecting_preference`, `socializing_preference` "
            "FROM `lw_profiles` WHERE `character_guid` = {}",
            characterGuid);

        if (!result)
            return std::nullopt;

        Field* fields = result->Fetch();
        BotProfile profile;
        profile.characterGuid = characterGuid;
        profile.seed = fields[0].Get<std::uint64_t>();
        profile.schemaVersion = fields[1].Get<std::uint16_t>();
        profile.primaryGoal = static_cast<GoalArchetype>(fields[2].Get<std::uint8_t>());
        profile.lifestyle.archetype = static_cast<ActivityArchetype>(fields[3].Get<std::uint8_t>());

        profile.personality.ambition = fields[4].Get<std::uint8_t>();
        profile.personality.sociability = fields[5].Get<std::uint8_t>();
        profile.personality.competitiveness = fields[6].Get<std::uint8_t>();
        profile.personality.curiosity = fields[7].Get<std::uint8_t>();
        profile.personality.loyalty = fields[8].Get<std::uint8_t>();
        profile.personality.patience = fields[9].Get<std::uint8_t>();
        profile.personality.riskTolerance = fields[10].Get<std::uint8_t>();

        profile.intelligence.tactical = fields[11].Get<std::uint8_t>();
        profile.intelligence.strategic = fields[12].Get<std::uint8_t>();
        profile.intelligence.social = fields[13].Get<std::uint8_t>();
        profile.intelligence.economic = fields[14].Get<std::uint8_t>();
        profile.intelligence.gameKnowledge = fields[15].Get<std::uint8_t>();
        profile.intelligence.adaptability = fields[16].Get<std::uint8_t>();
        profile.intelligence.attention = fields[17].Get<std::uint8_t>();

        profile.lifestyle.targetWeeklyHours = fields[18].Get<std::uint16_t>();
        profile.lifestyle.minimumSessionMinutes = fields[19].Get<std::uint16_t>();
        profile.lifestyle.maximumSessionMinutes = fields[20].Get<std::uint16_t>();
        profile.lifestyle.scheduleConsistency = fields[21].Get<std::uint8_t>();
        profile.lifestyle.skipSessionChance = fields[22].Get<std::uint8_t>();
        profile.lifestyle.marathonSessionChance = fields[23].Get<std::uint8_t>();
        profile.lifestyle.burnoutSensitivity = fields[24].Get<std::uint8_t>();

        profile.preferences.leveling = fields[25].Get<std::uint8_t>();
        profile.preferences.questing = fields[26].Get<std::uint8_t>();
        profile.preferences.dungeons = fields[27].Get<std::uint8_t>();
        profile.preferences.raiding = fields[28].Get<std::uint8_t>();
        profile.preferences.pvp = fields[29].Get<std::uint8_t>();
        profile.preferences.professions = fields[30].Get<std::uint8_t>();
        profile.preferences.collecting = fields[31].Get<std::uint8_t>();
        profile.preferences.socializing = fields[32].Get<std::uint8_t>();

        return profile;
    }

    bool ProfileManager::InsertProfile(BotProfile const& profile) const
    {
        CharacterDatabase.Execute(
            "INSERT IGNORE INTO `lw_profiles` ("
            "`character_guid`, `seed`, `schema_version`, `primary_goal`, `activity_archetype`, "
            "`ambition`, `sociability`, `competitiveness`, `curiosity`, `loyalty`, `patience`, `risk_tolerance`, "
            "`tactical_intelligence`, `strategic_intelligence`, `social_intelligence`, `economic_intelligence`, "
            "`game_knowledge`, `adaptability`, `attention`, "
            "`target_weekly_hours`, `minimum_session_minutes`, `maximum_session_minutes`, "
            "`schedule_consistency`, `skip_session_chance`, `marathon_session_chance`, `burnout_sensitivity`, "
            "`leveling_preference`, `questing_preference`, `dungeon_preference`, `raid_preference`, "
            "`pvp_preference`, `profession_preference`, `collecting_preference`, `socializing_preference`) "
            "VALUES ({}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {})",
            profile.characterGuid,
            profile.seed,
            profile.schemaVersion,
            static_cast<std::uint32_t>(profile.primaryGoal),
            static_cast<std::uint32_t>(profile.lifestyle.archetype),
            static_cast<std::uint32_t>(profile.personality.ambition),
            static_cast<std::uint32_t>(profile.personality.sociability),
            static_cast<std::uint32_t>(profile.personality.competitiveness),
            static_cast<std::uint32_t>(profile.personality.curiosity),
            static_cast<std::uint32_t>(profile.personality.loyalty),
            static_cast<std::uint32_t>(profile.personality.patience),
            static_cast<std::uint32_t>(profile.personality.riskTolerance),
            static_cast<std::uint32_t>(profile.intelligence.tactical),
            static_cast<std::uint32_t>(profile.intelligence.strategic),
            static_cast<std::uint32_t>(profile.intelligence.social),
            static_cast<std::uint32_t>(profile.intelligence.economic),
            static_cast<std::uint32_t>(profile.intelligence.gameKnowledge),
            static_cast<std::uint32_t>(profile.intelligence.adaptability),
            static_cast<std::uint32_t>(profile.intelligence.attention),
            profile.lifestyle.targetWeeklyHours,
            profile.lifestyle.minimumSessionMinutes,
            profile.lifestyle.maximumSessionMinutes,
            static_cast<std::uint32_t>(profile.lifestyle.scheduleConsistency),
            static_cast<std::uint32_t>(profile.lifestyle.skipSessionChance),
            static_cast<std::uint32_t>(profile.lifestyle.marathonSessionChance),
            static_cast<std::uint32_t>(profile.lifestyle.burnoutSensitivity),
            static_cast<std::uint32_t>(profile.preferences.leveling),
            static_cast<std::uint32_t>(profile.preferences.questing),
            static_cast<std::uint32_t>(profile.preferences.dungeons),
            static_cast<std::uint32_t>(profile.preferences.raiding),
            static_cast<std::uint32_t>(profile.preferences.pvp),
            static_cast<std::uint32_t>(profile.preferences.professions),
            static_cast<std::uint32_t>(profile.preferences.collecting),
            static_cast<std::uint32_t>(profile.preferences.socializing));

        return true;
    }
}
