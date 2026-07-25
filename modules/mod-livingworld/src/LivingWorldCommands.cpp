#include "LivingWorldPermissions.h"
#include "LivingWorldProfileManager.h"
#include "LivingWorldRuntimeState.h"
#include "LivingWorldSettings.h"

#include "Chat.h"
#include "CommandScript.h"
#include "Player.h"
#include "PlayerbotMgr.h"

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

        char const* PlannedActionName(PlannedPopulationAction action)
        {
            switch (action)
            {
                case PlannedPopulationAction::Login: return "login";
                case PlannedPopulationAction::Logout: return "logout";
                case PlannedPopulationAction::None: return "none";
            }
            return "none";
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
                "Online AI policy: minimum {} | target {} +/- {} | maximum {} | login/logout rates {}/{} per minute",
                settings.minimumOnline,
                settings.targetOnline,
                settings.populationTolerance,
                settings.maximumOnline,
                settings.loginRatePerMinute,
                settings.logoutRatePerMinute);
            handler->PSendSysMessage(
                "Compiled safety ceilings: population {} | login rate {} | logout rate {} per minute",
                SafetyLimits::MaximumPopulation,
                SafetyLimits::MaximumLoginRatePerMinute,
                SafetyLimits::MaximumLogoutRatePerMinute);
            handler->PSendSysMessage(
                "Loaded profiles: {}. Population execution remains in dry-run planning mode.",
                sLivingWorldProfiles.LoadedCount());

            if (!sLivingWorldRuntimeState.HasSamples())
            {
                handler->SendSysMessage("Runtime observations: no samples recorded since startup.");
                return;
            }

            RuntimeObservationSnapshot const& latest = sLivingWorldRuntimeState.Latest();
            RuntimeMetricSummary const summary = sLivingWorldRuntimeState.Summary();
            handler->PSendSysMessage(
                "Latest dry-run plan: action {} | AI/humans {}/{} | configured/effective target {}/{} | ceiling {} | requested/selected/unmet {}/{}/{}",
                PlannedActionName(latest.bounded.plannedAction),
                latest.bounded.onlineAI,
                latest.bounded.onlineHumans,
                latest.bounded.configuredTarget,
                latest.bounded.effectiveTarget,
                latest.bounded.effectiveAICeiling,
                latest.bounded.requestedOperations,
                latest.bounded.selectedCandidates,
                latest.unmetOperations);
            handler->PSendSysMessage(
                "Runtime window: {} samples | unhealthy {} | constrained {} | shortfall {} | emergency {} | average/peak update {}ms/{}ms",
                summary.sampleCount,
                summary.unhealthySamples,
                summary.constrainedTargetSamples,
                summary.candidateShortfallSamples,
                summary.emergencyDrainSamples,
                summary.averageUpdateTimeMs,
                summary.peakUpdateTimeMs);
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
                { "status", HandleStatusCommand, Permissions::View, Console::No },
                { "enable", HandleEnableCommand, Permissions::RuntimeControl, Console::No },
                { "disable", HandleDisableCommand, Permissions::RuntimeControl, Console::No },
                { "pause", HandlePauseCommand, Permissions::RuntimeControl, Console::No },
                { "resume", HandleResumeCommand, Permissions::RuntimeControl, Console::No },
                { "profile", HandleProfileCommand, Permissions::ProfileInspect, Console::No },
                { "population", HandlePopulationCommand, Permissions::PopulationControl, Console::No }
            };

            static ChatCommandTable commandTable =
            {
                { "livingworld", livingWorldCommandTable }
            };
            return commandTable;
        }

        static bool HandleStatusCommand(ChatHandler* handler, char const*)
        {
            PrintWorldStatus(handler);
            return true;
        }

        static bool HandleEnableCommand(ChatHandler* handler, char const*)
        {
            sLivingWorldSettings.SetOperationalState(true, true);
            handler->SendSysMessage("LivingWorld simulation and new AI logins are enabled.");
            return true;
        }

        static bool HandleDisableCommand(ChatHandler* handler, char const*)
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
                    "LivingWorld hard pause enabled. High-level simulation is frozen; forced Playerbot logout will be added with population execution.");
                return true;
            }

            handler->SendErrorMessage("Usage: .livingworld pause soft|hard");
            return false;
        }

        static bool HandleResumeCommand(ChatHandler* handler, char const*)
        {
            sLivingWorldSettings.SetOperationalState(true, true);
            handler->SendSysMessage("LivingWorld simulation and new AI logins are resumed.");
            return true;
        }

        static bool HandleProfileCommand(ChatHandler* handler, char const*)
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
                if (value == "on" || value == "off")
                {
                    sLivingWorldSettings.SetAutoScale(value == "on");
                    handler->PSendSysMessage("LivingWorld population auto-scale is {}.", value == "on" ? "enabled" : "disabled");
                    return true;
                }
                handler->SendErrorMessage("Usage: .livingworld population auto on|off");
                return false;
            }

            std::uint32_t value = 0;
            if (!(input >> value))
            {
                handler->SendErrorMessage("Usage: .livingworld population min|max|target|tolerance <number>");
                return false;
            }

            bool accepted = false;
            if (action == "min")
                accepted = sLivingWorldSettings.SetMinimumOnline(value);
            else if (action == "max")
                accepted = sLivingWorldSettings.SetMaximumOnline(value);
            else if (action == "target")
                accepted = sLivingWorldSettings.SetTargetOnline(value);
            else if (action == "tolerance")
                accepted = sLivingWorldSettings.SetPopulationTolerance(value);
            else
            {
                handler->SendErrorMessage("Usage: .livingworld population [min|max|target|tolerance <number>|auto on|off]");
                return false;
            }

            if (!accepted)
            {
                handler->PSendSysMessage(
                    "Rejected unsafe or inconsistent population value. Population values cannot exceed {} and must preserve minimum <= target <= maximum.",
                    SafetyLimits::MaximumPopulation);
                return false;
            }

            handler->PSendSysMessage("LivingWorld population {} set to {}.", action, value);
            return true;
        }
    };
}

void AddLivingWorldCommandScripts()
{
    new LivingWorld::LivingWorldCommandScript();
}
