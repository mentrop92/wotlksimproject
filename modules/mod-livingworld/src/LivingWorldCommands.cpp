#include "LivingWorldProfileManager.h"

#include "Chat.h"
#include "CommandScript.h"
#include "Player.h"
#include "PlayerbotMgr.h"
#include "RBAC.h"

using namespace Acore::ChatCommands;

namespace LivingWorld
{
    namespace
    {
        char const* ActivityName(ActivityArchetype archetype)
        {
            switch (archetype)
            {
                case ActivityArchetype::Dormant: return "Dormant";
                case ActivityArchetype::Casual: return "Casual";
                case ActivityArchetype::Weekend: return "Weekend";
                case ActivityArchetype::Regular: return "Regular";
                case ActivityArchetype::Dedicated: return "Dedicated";
                case ActivityArchetype::Hardcore: return "Hardcore";
                case ActivityArchetype::NoLifeGrinder: return "No-life grinder";
            }

            return "Unknown";
        }

        char const* GoalName(GoalArchetype goal)
        {
            switch (goal)
            {
                case GoalArchetype::Leveling: return "Leveling";
                case GoalArchetype::Dungeons: return "Dungeons";
                case GoalArchetype::Raiding: return "Raiding";
                case GoalArchetype::PvP: return "PvP";
                case GoalArchetype::Economy: return "Economy";
                case GoalArchetype::Professions: return "Professions";
                case GoalArchetype::Collecting: return "Collecting";
                case GoalArchetype::SocialLeadership: return "Social leadership";
            }

            return "Unknown";
        }
    }

    class LivingWorldCommandScript : public CommandScript
    {
    public:
        LivingWorldCommandScript() : CommandScript("LivingWorldCommandScript") { }

        ChatCommandTable GetCommands() const override
        {
            static ChatCommandTable livingWorldCommandTable =
            {
                { "profile", HandleProfileCommand, rbac::RBAC_PERM_COMMAND_GM, Console::No },
                { "population", HandlePopulationCommand, rbac::RBAC_PERM_COMMAND_GM, Console::No }
            };

            static ChatCommandTable commandTable =
            {
                { "livingworld", livingWorldCommandTable }
            };

            return commandTable;
        }

        static bool HandleProfileCommand(ChatHandler* handler, char const* /*args*/)
        {
            Player* target = handler->getSelectedPlayerOrSelf();
            if (!target)
            {
                handler->SendErrorMessage("Select an online player or Playerbot first.");
                return false;
            }

            std::uint32_t const guid = target->GetGUID().GetCounter();
            BotProfile const* profile = sLivingWorldProfiles.GetProfile(guid);

            if (!profile && sPlayerbotsMgr.GetPlayerbotAI(target) != nullptr)
                profile = sLivingWorldProfiles.EnsureProfile(target);

            if (!profile)
            {
                handler->SendErrorMessage("{} does not have a loaded LivingWorld profile.", target->GetName());
                return false;
            }

            handler->PSendSysMessage("LivingWorld profile: {} (GUID {})", target->GetName(), guid);
            handler->PSendSysMessage("Schema: {} | Seed: {}", profile->schemaVersion, profile->seed);
            handler->PSendSysMessage(
                "Lifestyle: {} | Target: {} hours/week | Sessions: {}-{} minutes",
                ActivityName(profile->lifestyle.archetype),
                profile->lifestyle.targetWeeklyHours,
                profile->lifestyle.minimumSessionMinutes,
                profile->lifestyle.maximumSessionMinutes);
            handler->PSendSysMessage(
                "Personality: ambition {} | sociability {} | competitiveness {} | curiosity {} | loyalty {} | patience {} | risk {}",
                static_cast<std::uint32_t>(profile->personality.ambition),
                static_cast<std::uint32_t>(profile->personality.sociability),
                static_cast<std::uint32_t>(profile->personality.competitiveness),
                static_cast<std::uint32_t>(profile->personality.curiosity),
                static_cast<std::uint32_t>(profile->personality.loyalty),
                static_cast<std::uint32_t>(profile->personality.patience),
                static_cast<std::uint32_t>(profile->personality.riskTolerance));
            handler->PSendSysMessage(
                "Intelligence: tactical {} | strategic {} | social {} | economic {} | knowledge {} | adaptability {} | attention {}",
                static_cast<std::uint32_t>(profile->intelligence.tactical),
                static_cast<std::uint32_t>(profile->intelligence.strategic),
                static_cast<std::uint32_t>(profile->intelligence.social),
                static_cast<std::uint32_t>(profile->intelligence.economic),
                static_cast<std::uint32_t>(profile->intelligence.gameKnowledge),
                static_cast<std::uint32_t>(profile->intelligence.adaptability),
                static_cast<std::uint32_t>(profile->intelligence.attention));
            handler->PSendSysMessage(
                "Preferences: leveling {} | questing {} | dungeons {} | raids {} | PvP {} | professions {} | collecting {} | socializing {}",
                static_cast<std::uint32_t>(profile->preferences.leveling),
                static_cast<std::uint32_t>(profile->preferences.questing),
                static_cast<std::uint32_t>(profile->preferences.dungeons),
                static_cast<std::uint32_t>(profile->preferences.raiding),
                static_cast<std::uint32_t>(profile->preferences.pvp),
                static_cast<std::uint32_t>(profile->preferences.professions),
                static_cast<std::uint32_t>(profile->preferences.collecting),
                static_cast<std::uint32_t>(profile->preferences.socializing));
            handler->PSendSysMessage("Primary goal: {}", GoalName(profile->primaryGoal));
            return true;
        }

        static bool HandlePopulationCommand(ChatHandler* handler, char const* /*args*/)
        {
            handler->PSendSysMessage("LivingWorld currently has {} profiles loaded in memory.", sLivingWorldProfiles.LoadedCount());
            return true;
        }
    };
}

void AddLivingWorldCommandScripts()
{
    new LivingWorld::LivingWorldCommandScript();
}
