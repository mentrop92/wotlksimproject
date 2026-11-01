#include "LivingWorldPopulationController.h"
#include "LivingWorldPopulationExecution.h"
#include "LivingWorldProfile.h"
#include "LivingWorldProfileManager.h"
#include "LivingWorldRuntimeState.h"
#include "LivingWorldSettings.h"

#include "Config.h"
#include "Group.h"
#include "Log.h"
#include "Map.h"
#include "Player.h"
#include "PlayerbotAIConfig.h"
#include "PlayerbotMgr.h"
#include "RandomPlayerbotMgr.h"
#include "ScriptMgr.h"
#include "WorldSession.h"
#include "WorldSessionMgr.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

void AddLivingWorldCommandScripts();

namespace LivingWorld
{
    namespace Config
    {
        bool Enabled = true;
        bool ProfileHumans = false;
        std::uint32_t ProfileCheckIntervalMs = 5000;
        std::uint32_t ProfileDetectionTimeoutMs = 60000;
        std::uint32_t MemoryLimit = 50;
        bool ExecutePopulation = false;
    }

    namespace
    {
        struct PendingProfileCheck
        {
            std::uint32_t intervalElapsed = 0;
            std::uint32_t totalElapsed = 0;
        };

        bool IsEligibleHuman(Player* player)
        {
            return Config::ProfileHumans && player && !player->IsGameMaster();
        }

        bool IsEligiblePlayerbot(Player* player)
        {
            return player && !player->IsGameMaster() && sPlayerbotsMgr.GetPlayerbotAI(player) != nullptr;
        }

        bool MayProvisionProfiles()
        {
            WorldSettings const& settings = sLivingWorldSettings.Get();
            return Config::Enabled && settings.simulationEnabled && settings.loginEnabled;
        }

        PlannedPopulationAction ToObservedAction(PopulationAction action)
        {
            switch (action)
            {
                case PopulationAction::Login: return PlannedPopulationAction::Login;
                case PopulationAction::Logout: return PlannedPopulationAction::Logout;
                case PopulationAction::None: return PlannedPopulationAction::None;
            }
            return PlannedPopulationAction::None;
        }

        // Playerbot sessions are created without a socket and are never added
        // to WorldSessionMgr, so every in-world player there is human-controlled.
        // The Playerbot check stays as a guard against future registration changes.
        std::uint32_t CountOnlineHumans()
        {
            std::uint32_t humans = 0;
            for (auto const& [accountId, session] : sWorldSessionMgr->GetAllSessions())
            {
                Player* player = session ? session->GetPlayer() : nullptr;
                if (player && player->IsInWorld() && !sPlayerbotsMgr.GetPlayerbotAI(player))
                    ++humans;
            }
            return humans;
        }

        bool IsGroupedWithHuman(Player* bot)
        {
            Group* group = bot->GetGroup();
            if (!group)
                return false;

            for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
            {
                Player* member = ref->GetSource();
                if (member && member != bot && !sPlayerbotsMgr.GetPlayerbotAI(member))
                    return true;
            }
            return false;
        }

        struct RandomBotSnapshot
        {
            // Online random bots that are not already waiting for Playerbots
            // to process a cleared "add" event.
            std::uint32_t remainingOnline = 0;
            std::vector<PopulationCandidate> logoutCandidates;
        };

        RandomBotSnapshot CollectRandomBots()
        {
            RandomBotSnapshot snapshot;
            snapshot.logoutCandidates.reserve(sRandomPlayerbotMgr.GetPlayerbotsCount());
            auto const end = sRandomPlayerbotMgr.GetPlayerBotsEnd();
            for (auto itr = sRandomPlayerbotMgr.GetPlayerBotsBegin(); itr != end; ++itr)
            {
                Player* bot = itr->second;
                if (!bot || !bot->IsInWorld())
                    continue;

                // A bot whose "add" was cleared is already leaving. Counting it as
                // online would re-select it, or hold bot_count high enough for the
                // manager to log in a replacement before it is gone.
                if (!sRandomPlayerbotMgr.GetValue(bot->GetGUID().GetCounter(), "add"))
                    continue;

                ++snapshot.remainingOnline;

                Group* group = bot->GetGroup();

                PopulationCandidate candidate;
                candidate.characterGuid = bot->GetGUID().GetCounter();
                candidate.online = true;
                candidate.inCombat = bot->IsInCombat();
                candidate.inInstance = bot->GetMap() && bot->GetMap()->Instanceable();
                candidate.grouped = group != nullptr;
                candidate.groupLeader = group && group->IsLeader(bot->GetGUID());
                candidate.interactingWithHuman = IsGroupedWithHuman(bot);
                // RandomPlayerbotMgr refuses to log out grouped bots, so they
                // are protected rather than selected and silently retained.
                candidate.protectedActivity = group != nullptr || bot->IsInFlight();
                snapshot.logoutCandidates.push_back(candidate);
            }
            return snapshot;
        }

        bool MayExecutePopulation(WorldSettings const& settings)
        {
            // Paused or manual policy hands bot_count back to Playerbots' own
            // randomized target once the last steered value expires.
            return Config::ExecutePopulation && settings.simulationEnabled && settings.autoScale &&
                sPlayerbotAIConfig.enabled && sPlayerbotAIConfig.randomBotAutologin;
        }

        // Returns the number of bots selected for logout.
        std::uint32_t ExecutePopulationDecision(PopulationDecision const& decision, RandomBotSnapshot const& snapshot)
        {
            std::uint32_t const onlineRandomBots = snapshot.remainingOnline;
            PlayerbotPopulationLimits const limits = {
                sPlayerbotAIConfig.minRandomBots,
                sPlayerbotAIConfig.maxRandomBots
            };
            PopulationExecutionPlan const plan = PopulationExecutionPlanner::Plan(
                decision,
                onlineRandomBots,
                sRandomPlayerbotMgr.GetMaxAllowedBotCount(),
                limits,
                snapshot.logoutCandidates);

            // Clearing "add" makes RandomPlayerbotMgr log the bot out on its next
            // pass through its own safety checks.
            for (std::uint32_t guid : plan.logoutGuids)
                sRandomPlayerbotMgr.SetValue(guid, "add", 0);

            if (plan.writeBotCountTarget)
            {
                // Owner 0 holds RandomPlayerbotMgr's global events.
                sRandomPlayerbotMgr.SetValue(std::uint32_t(0), "bot_count", plan.botCountTarget);
                LOG_DEBUG(
                    "module.livingworld",
                    "Population execution set Playerbots bot_count to {} ({} online, {} logout(s) requested{}).",
                    plan.botCountTarget,
                    onlineRandomBots,
                    plan.logoutGuids.size(),
                    plan.clampedToPlayerbotLimits ? ", clamped to Playerbots min/max random bots" : "");
            }

            return static_cast<std::uint32_t>(plan.logoutGuids.size());
        }
    }

    class LivingWorldWorldScript : public WorldScript
    {
    public:
        LivingWorldWorldScript() : WorldScript("LivingWorldWorldScript", {
            WORLDHOOK_ON_BEFORE_CONFIG_LOAD,
            WORLDHOOK_ON_STARTUP
        }) { }

        void OnBeforeConfigLoad(bool /*reload*/) override
        {
            Config::Enabled = sConfigMgr->GetOption<bool>("LivingWorld.Enable", true);
            Config::ProfileHumans = sConfigMgr->GetOption<bool>("LivingWorld.ProfileHumans", false);
            Config::ProfileCheckIntervalMs = sConfigMgr->GetOption<std::uint32_t>("LivingWorld.ProfileCheckIntervalMs", 5000);
            Config::ProfileDetectionTimeoutMs = sConfigMgr->GetOption<std::uint32_t>("LivingWorld.ProfileDetectionTimeoutMs", 60000);
            Config::MemoryLimit = sConfigMgr->GetOption<std::uint32_t>("LivingWorld.MemoryLimit", 50);
            Config::ExecutePopulation = sConfigMgr->GetOption<bool>("LivingWorld.Population.Execute", false);
        }

        void OnStartup() override
        {
            if (!Config::Enabled)
            {
                LOG_INFO("module.livingworld", "LivingWorld is disabled by configuration.");
                return;
            }

            sLivingWorldSettings.Load();
            sLivingWorldPopulation.Reset();
            sLivingWorldRuntimeState.Reset();
            WorldSettings const& settings = sLivingWorldSettings.Get();

            BotProfile const sample = ProfileGenerator::Generate(1, 1, 1);
            LOG_INFO(
                "module.livingworld",
                "LivingWorld started. Profile schema v{}, deterministic generator ready (sample seed {}).",
                sample.schemaVersion,
                sample.seed);
            LOG_INFO(
                "module.livingworld",
                "LivingWorld runtime: simulation {}, logins {}, population min/target/max {}/{}/{}.",
                settings.simulationEnabled ? "enabled" : "paused",
                settings.loginEnabled ? "enabled" : "paused",
                settings.minimumOnline,
                settings.targetOnline,
                settings.maximumOnline);
        }
    };

    class LivingWorldPlayerbotScript : public PlayerbotScript
    {
    public:
        LivingWorldPlayerbotScript() : PlayerbotScript("LivingWorldPlayerbotScript") { }

        void OnPlayerbotUpdate(std::uint32_t diff) override
        {
            if (!Config::Enabled)
                return;

            // The hook runs every world tick; population state changes on the
            // scale of seconds, and the controller's rate credit is diff-based.
            _elapsedMs += diff;
            if (_elapsedMs < PopulationUpdateIntervalMs)
                return;

            std::uint32_t const elapsedMs = _elapsedMs;
            _elapsedMs = 0;

            WorldSettings const& settings = sLivingWorldSettings.Get();
            bool const execute = MayExecutePopulation(settings);
            RandomBotSnapshot const snapshot = execute
                ? CollectRandomBots()
                : RandomBotSnapshot{ sRandomPlayerbotMgr.GetPlayerbotsCount(), {} };
            PopulationDecision const decision = sLivingWorldPopulation.Update(
                elapsedMs,
                snapshot.remainingOnline,
                CountOnlineHumans(),
                settings);

            RuntimeObservationInput observation;
            observation.onlineAI = decision.currentOnline;
            observation.onlineHumans = decision.currentHumanOnline;
            observation.configuredTarget = settings.targetOnline;
            observation.effectiveTarget = decision.effectiveTarget;
            observation.effectiveAICeiling = decision.effectiveAICeiling;
            observation.plannedAction = ToObservedAction(decision.action);
            observation.requestedOperations = decision.requestedCount;
            // Dry run selects nothing; reporting zero keeps that explicit
            // rather than fabricating work.
            observation.selectedCandidates = execute ? ExecutePopulationDecision(decision, snapshot) : 0;
            observation.averageUpdateTimeMs = diff;
            sLivingWorldRuntimeState.Record(observation);

            if (decision.requestedCount == 0)
                return;

            LOG_DEBUG(
                "module.livingworld",
                "Population controller planned {} {} operation(s): AI online {}, humans {}, target {}, AI ceiling {}{}.",
                decision.requestedCount,
                decision.action == PopulationAction::Login ? "login" : "logout",
                decision.currentOnline,
                decision.currentHumanOnline,
                decision.effectiveTarget,
                decision.effectiveAICeiling,
                execute ? "" : " (dry run: execution disabled or paused)");
        }

    private:
        static constexpr std::uint32_t PopulationUpdateIntervalMs = 1000;
        std::uint32_t _elapsedMs = 0;
    };

    class LivingWorldPlayerScript : public PlayerScript
    {
    public:
        LivingWorldPlayerScript() : PlayerScript("LivingWorldPlayerScript", {
            PLAYERHOOK_ON_LOGIN,
            PLAYERHOOK_ON_UPDATE,
            PLAYERHOOK_ON_LOGOUT
        }) { }

        void OnPlayerLogin(Player* player) override
        {
            if (!MayProvisionProfiles() || !player || player->IsGameMaster())
                return;

            std::uint32_t const guid = player->GetGUID().GetCounter();

            if (IsEligibleHuman(player) || IsEligiblePlayerbot(player))
            {
                sLivingWorldProfiles.EnsureProfile(player);
                return;
            }

            _pendingChecks[guid] = PendingProfileCheck{};
        }

        void OnPlayerUpdate(Player* player, std::uint32_t diff) override
        {
            if (!MayProvisionProfiles() || !player)
                return;

            std::uint32_t const guid = player->GetGUID().GetCounter();
            auto itr = _pendingChecks.find(guid);
            if (itr == _pendingChecks.end())
                return;

            PendingProfileCheck& pending = itr->second;
            pending.intervalElapsed += diff;
            pending.totalElapsed += diff;

            if (pending.intervalElapsed < Config::ProfileCheckIntervalMs)
                return;

            pending.intervalElapsed = 0;

            if (IsEligibleHuman(player) || IsEligiblePlayerbot(player))
            {
                sLivingWorldProfiles.EnsureProfile(player);
                _pendingChecks.erase(itr);
                return;
            }

            if (pending.totalElapsed >= Config::ProfileDetectionTimeoutMs)
                _pendingChecks.erase(itr);
        }

        void OnPlayerLogout(Player* player) override
        {
            if (!player)
                return;

            std::uint32_t const guid = player->GetGUID().GetCounter();
            _pendingChecks.erase(guid);
            sLivingWorldProfiles.UnloadProfile(guid, true);
        }

    private:
        std::unordered_map<std::uint32_t, PendingProfileCheck> _pendingChecks;
    };
}

void AddLivingWorldScripts()
{
    new LivingWorld::LivingWorldWorldScript();
    new LivingWorld::LivingWorldPlayerbotScript();
    new LivingWorld::LivingWorldPlayerScript();
    AddLivingWorldCommandScripts();
}
