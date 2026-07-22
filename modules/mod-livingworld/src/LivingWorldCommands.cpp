#include "LivingWorldProfileManager.h"
#include "LivingWorldSettings.h"

#include "Chat.h"
#include "CommandScript.h"
#include "Player.h"
#include "PlayerbotMgr.h"
#include "RBAC.h"

#include <sstream>
#include <string>

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

        void PrintWorldStatus(ChatHandler* handler)
        {
            WorldSettings const& settings = sLivingWorldSettings.Get();
            handler->PSendSysMessage(
                "LivingWorld: simulation {} | new AI logins {} | auto-scale {}",
                settings.simulationEnabled ? "enabled" : "paused",
                settings.loginEnabled ? "enabled" : "paused",
                settings.autoScale ? "enabled" : "disabled");
            handler->PSendSysMessage(
                "Online AI policy: minimum {} | target {} | maximum {} | login/logout rates {}/{} per minute",
                settings.minimumOnline,
                settings.targetOnline,
                settings.maximumOnline,
                settings.loginRatePerMinute,
                settings.logoutRatePerMinute);
            handler->PSendSysMessage(
                "Loaded profiles: {}. Automatic population adjustment is planned for the scheduling phase.",
                sLivingWorldProfiles.LoadedCount());
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
                { "status", HandleStatusCommand, rbac::RBAC_PERM_COMMAND_GM, Console::No },
                { "enable", HandleEnableCommand, rbac::RBAC_PERM_COMMAND_GM, Console::No },
                { "disable", HandleDisableCommand, rbac::RBAC_PERM_COMMAND_GM, Console::No },
                { "pause", HandlePauseCommand, rbac::RBAC_PERM_COMMAND_GM, Console::No },
                { "resume", HandleResumeCommand, rbac::RBAC_PERM_COMMAND_GM, Console::No },
                { "profile", HandleProfileCommand, rbac::RBAC_PERM_COMMAND_GM, Console::No },
                { "population", HandlePopulationCommand, rbac::RBAC_PERM_COMMAND_GM, Console::No }
            };

            static ChatCommandTable commandTable =
            {
                { "livingworld", livingWorldCommandTable }
            };

            return commandTable;
        }

        static bool HandleStatusCommand(ChatHandler* handler, char const* /*args*/)
        {
            PrintWorldStatus(handler);
            return true;
        }

        static bool HandleEnableCommand(ChatHandler* handler, char const* /*args*/)
        {
            sLivingWorldSettings.SetOperationalState(true, true);
            handler->SendSysMessage("LivingWorld simulation and new AI logins are enabled.");
            return true;
        }

        static bool HandleDisableCommand(ChatHandler* handler, char const* /*args*/)
        {
            sLivingWorldSettings.SetOperationalState(false, false);
            handler->SendSysMessage(
                "LivingWorld is disabled. Existing Playerbots are not forcibly logged out by this foundation command.");
            return true;
        }

        static bool HandlePauseCommand(ChatHandler* handler, char const* args)
        {
            std::istringstream input(args ? args : "");
            std::string mode;
            input >> mode;

            if (mode == "soft")
            {
                sLivingWorldSettings.SetOperationalState(true, false);
                handler->SendSysMessage(
                    "LivingWorld soft pause enabled: no new LivingWorld AI profiles or scheduled logins will begin.");
                return true;
            }

            if (mode == "hard")
            {
                sLivingWorldSettings.SetOperationalState(false, false);
                handler->SendSysMessage(
                    "LivingWorld hard pause enabled. High-level simulation is frozen; forced Playerbot logout will be added with the population controller.");
                return true;
            }

            handler->SendErrorMessage("Usage: .livingworld pause soft|hard");
            return false;
        }

        static bool HandleResumeCommand(ChatHandler* handler, char const* /*args*/)
        {
            sLivingWorldSettings.SetOperationalState(true, true);
            handler->SendSysMessage("LivingWorld simulation and new AI logins are resumed.");
            return true;
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
            {
                WorldSettings const& settings = sLivingWorldSettings.Get();
                if (!settings.simulationEnabled || !settings.loginEnabled)
                {
                    handler->SendErrorMessage(
                        "LivingWorld is paused; resume it before creating a new profile for {}.", target->GetName());
                    return false;
                }

                profile = sLivingWorldProfiles.EnsureProfile(target);
            }

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

        static bool HandlePopulationCommand(ChatHandler* handler, char const* args)
        {
            std::istringstream input(args ? args : "");
            std::string action;
            input >> action;

            if (action.empty())
            {
                PrintWorldStatus(handler);
                return true;
            }

            if (action == "auto")
            {
                std::string value;
                input >> value;
                if (value == "on")
                {
                    sLivingWorldSettings.SetAutoScale(true);
                    handler->SendSysMessage("LivingWorld population auto-scale is enabled.");
                    return true;
                }

                if (value == "off")
                {
                    sLivingWorldSettings.SetAutoScale(false);
                    handler->SendSysMessage("LivingWorld population auto-scale is disabled.");
                    return true;
                }

                handler->SendErrorMessage("Usage: .livingworld population auto on|off");
                return false;
            }

            std::uint32_t value = 0;
            if (!(input >> value))
            {
                handler->SendErrorMessage("Usage: .livingworld population min|max|target <number>");
                return false;
            }

            if (action == "min")
            {
                if (!sLivingWorldSettings.SetMinimumOnline(value))
                {
                    handler->SendErrorMessage("Minimum must not exceed the current target or maximum.");
                    return false;
                }

                handler->PSendSysMessage("LivingWorld minimum online AI population set to {}.", value);
                return true;
            }

            if (action == "max")
            {
                if (!sLivingWorldSettings.SetMaximumOnline(value))
                {
                    handler->SendErrorMessage("Maximum must not be below the current minimum or target.");
                    return false;
                }

                handler->PSendSysMessage("LivingWorld maximum online AI population set to {}.", value);
                return true;
            }

            if (action == "target")
            {
                if (!sLivingWorldSettings.SetTargetOnline(value))
                {
                    handler->SendErrorMessage("Target must remain between the current minimum and maximum.");
                    return false;
                }

                handler->PSendSysMessage("LivingWorld target online AI population set to {}.", value);
                return true;
            }

            handler->SendErrorMessage("Usage: .livingworld population [min|max|target <number>|auto on|off]");
            return false;
        }
    };
}

void AddLivingWorldCommandScripts()
{
    new LivingWorld::LivingWorldCommandScript();
}
