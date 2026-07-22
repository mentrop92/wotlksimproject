#include "LivingWorldProfile.h"
#include "LivingWorldProfileManager.h"

#include "Config.h"
#include "Log.h"
#include "Player.h"
#include "PlayerbotMgr.h"
#include "ScriptMgr.h"

#include <cstdint>
#include <unordered_map>

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
        }

        void OnStartup() override
        {
            if (!Config::Enabled)
            {
                LOG_INFO("module.livingworld", "LivingWorld is disabled.");
                return;
            }

            BotProfile const sample = ProfileGenerator::Generate(1, 1, 1);
            LOG_INFO(
                "module.livingworld",
                "LivingWorld started. Profile schema v{}, deterministic generator ready (sample seed {}).",
                sample.schemaVersion,
                sample.seed);
        }
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
            if (!Config::Enabled || !player || player->IsGameMaster())
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
            if (!Config::Enabled || !player)
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
    new LivingWorld::LivingWorldPlayerScript();
    AddLivingWorldCommandScripts();
}
