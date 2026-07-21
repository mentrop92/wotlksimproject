#include "LivingWorldProfile.h"

#include "Config.h"
#include "Log.h"
#include "ScriptMgr.h"

namespace LivingWorld
{
    namespace Config
    {
        bool Enabled = true;
        bool ProfileHumans = false;
        std::uint32_t ProfileCheckIntervalMs = 5000;
        std::uint32_t MemoryLimit = 50;
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
            Config::MemoryLimit = sConfigMgr->GetOption<std::uint32_t>("LivingWorld.MemoryLimit", 50);
        }

        void OnStartup() override
        {
            if (!Config::Enabled)
            {
                LOG_INFO("module.livingworld", "LivingWorld is disabled.");
                return;
            }

            // A deterministic smoke sample proves the generator is linked and
            // gives CI/startup logs a stable value that can be checked later.
            BotProfile const sample = ProfileGenerator::Generate(1, 1, 1);
            LOG_INFO(
                "module.livingworld",
                "LivingWorld started. Profile schema v{}, deterministic generator ready (sample seed {}).",
                sample.schemaVersion,
                sample.seed);
        }
    };
}

void AddLivingWorldScripts()
{
    new LivingWorld::LivingWorldWorldScript();
}
