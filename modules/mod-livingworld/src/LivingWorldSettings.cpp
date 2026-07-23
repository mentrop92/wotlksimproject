#include "LivingWorldSettings.h"

#include "Config.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "QueryResult.h"

#include <algorithm>

namespace LivingWorld
{
    WorldSettingsManager& WorldSettingsManager::Instance()
    {
        static WorldSettingsManager instance;
        return instance;
    }

    void WorldSettingsManager::Load()
    {
        LoadDefaultsFromConfig();

        QueryResult result = CharacterDatabase.Query(
            "SELECT `simulation_enabled`, `login_enabled`, `minimum_online`, `maximum_online`, "
            "`target_online`, `population_tolerance`, `auto_scale`, `login_rate_per_minute`, `logout_rate_per_minute`, "
            "`population_mode`, `seed_state` FROM `lw_world_settings` WHERE `settings_id` = 1");

        if (!result)
        {
            CharacterDatabase.Execute(
                "INSERT INTO `lw_world_settings` ("
                "`settings_id`, `simulation_enabled`, `login_enabled`, `minimum_online`, `maximum_online`, "
                "`target_online`, `population_tolerance`, `auto_scale`, `login_rate_per_minute`, `logout_rate_per_minute`, "
                "`population_mode`, `seed_state`) "
                "VALUES (1, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {})",
                _settings.simulationEnabled ? 1U : 0U,
                _settings.loginEnabled ? 1U : 0U,
                _settings.minimumOnline,
                _settings.maximumOnline,
                _settings.targetOnline,
                _settings.populationTolerance,
                _settings.autoScale ? 1U : 0U,
                _settings.loginRatePerMinute,
                _settings.logoutRatePerMinute,
                static_cast<std::uint32_t>(_settings.populationMode),
                static_cast<std::uint32_t>(_settings.seedState));

            LOG_INFO("module.livingworld", "Created default LivingWorld world settings.");
            return;
        }

        Field* fields = result->Fetch();
        _settings.simulationEnabled = fields[0].Get<std::uint8_t>() != 0;
        _settings.loginEnabled = fields[1].Get<std::uint8_t>() != 0;
        _settings.minimumOnline = fields[2].Get<std::uint32_t>();
        _settings.maximumOnline = fields[3].Get<std::uint32_t>();
        _settings.targetOnline = fields[4].Get<std::uint32_t>();
        _settings.populationTolerance = fields[5].Get<std::uint32_t>();
        _settings.autoScale = fields[6].Get<std::uint8_t>() != 0;
        _settings.loginRatePerMinute = fields[7].Get<std::uint32_t>();
        _settings.logoutRatePerMinute = fields[8].Get<std::uint32_t>();
        _settings.populationMode = static_cast<PopulationMode>(fields[9].Get<std::uint8_t>());
        _settings.seedState = static_cast<SeedState>(fields[10].Get<std::uint8_t>());

        WorldSettings const before = _settings;
        Normalize();

        if (before.minimumOnline != _settings.minimumOnline || before.maximumOnline != _settings.maximumOnline ||
            before.targetOnline != _settings.targetOnline || before.populationTolerance != _settings.populationTolerance ||
            before.loginRatePerMinute != _settings.loginRatePerMinute || before.logoutRatePerMinute != _settings.logoutRatePerMinute)
        {
            LOG_WARN("module.livingworld", "Unsafe persisted LivingWorld settings were clamped to compiled safety limits.");
            Persist();
        }
    }

    WorldSettings const& WorldSettingsManager::Get() const
    {
        return _settings;
    }

    void WorldSettingsManager::SetOperationalState(bool simulationEnabled, bool loginEnabled)
    {
        _settings.simulationEnabled = simulationEnabled;
        _settings.loginEnabled = loginEnabled;
        Persist();
    }

    bool WorldSettingsManager::SetMinimumOnline(std::uint32_t value)
    {
        if (value > SafetyLimits::MaximumPopulation || value > _settings.maximumOnline || value > _settings.targetOnline)
            return false;

        _settings.minimumOnline = value;
        Normalize();
        Persist();
        return true;
    }

    bool WorldSettingsManager::SetMaximumOnline(std::uint32_t value)
    {
        if (value > SafetyLimits::MaximumPopulation || value < _settings.minimumOnline || value < _settings.targetOnline)
            return false;

        _settings.maximumOnline = value;
        Normalize();
        Persist();
        return true;
    }

    bool WorldSettingsManager::SetTargetOnline(std::uint32_t value)
    {
        if (value > SafetyLimits::MaximumPopulation || value < _settings.minimumOnline || value > _settings.maximumOnline)
            return false;

        _settings.targetOnline = value;
        Normalize();
        Persist();
        return true;
    }

    bool WorldSettingsManager::SetPopulationTolerance(std::uint32_t value)
    {
        if (value > SafetyLimits::MaximumPopulation || value > _settings.maximumOnline - _settings.minimumOnline)
            return false;

        _settings.populationTolerance = value;
        Normalize();
        Persist();
        return true;
    }

    void WorldSettingsManager::SetAutoScale(bool enabled)
    {
        _settings.autoScale = enabled;
        Persist();
    }

    void WorldSettingsManager::LoadDefaultsFromConfig()
    {
        _settings.simulationEnabled = sConfigMgr->GetOption<bool>("LivingWorld.RuntimeEnabled", true);
        _settings.loginEnabled = sConfigMgr->GetOption<bool>("LivingWorld.LoginEnabled", true);
        _settings.minimumOnline = sConfigMgr->GetOption<std::uint32_t>("LivingWorld.Population.MinimumOnline", 50);
        _settings.maximumOnline = sConfigMgr->GetOption<std::uint32_t>("LivingWorld.Population.MaximumOnline", 500);
        _settings.targetOnline = sConfigMgr->GetOption<std::uint32_t>("LivingWorld.Population.TargetOnline", 250);
        _settings.populationTolerance = sConfigMgr->GetOption<std::uint32_t>("LivingWorld.Population.Tolerance", 15);
        _settings.autoScale = sConfigMgr->GetOption<bool>("LivingWorld.Population.AutoScale", true);
        _settings.loginRatePerMinute = sConfigMgr->GetOption<std::uint32_t>("LivingWorld.Population.LoginRatePerMinute", 15);
        _settings.logoutRatePerMinute = sConfigMgr->GetOption<std::uint32_t>("LivingWorld.Population.LogoutRatePerMinute", 20);
        Normalize();
    }

    void WorldSettingsManager::Persist() const
    {
        CharacterDatabase.Execute(
            "UPDATE `lw_world_settings` SET "
            "`simulation_enabled` = {}, `login_enabled` = {}, `minimum_online` = {}, `maximum_online` = {}, "
            "`target_online` = {}, `population_tolerance` = {}, `auto_scale` = {}, "
            "`login_rate_per_minute` = {}, `logout_rate_per_minute` = {}, "
            "`population_mode` = {}, `seed_state` = {} WHERE `settings_id` = 1",
            _settings.simulationEnabled ? 1U : 0U,
            _settings.loginEnabled ? 1U : 0U,
            _settings.minimumOnline,
            _settings.maximumOnline,
            _settings.targetOnline,
            _settings.populationTolerance,
            _settings.autoScale ? 1U : 0U,
            _settings.loginRatePerMinute,
            _settings.logoutRatePerMinute,
            static_cast<std::uint32_t>(_settings.populationMode),
            static_cast<std::uint32_t>(_settings.seedState));
    }

    void WorldSettingsManager::Normalize()
    {
        _settings.minimumOnline = std::min(_settings.minimumOnline, SafetyLimits::MaximumPopulation);
        _settings.maximumOnline = std::clamp(
            _settings.maximumOnline,
            _settings.minimumOnline,
            SafetyLimits::MaximumPopulation);
        _settings.targetOnline = std::clamp(_settings.targetOnline, _settings.minimumOnline, _settings.maximumOnline);
        _settings.populationTolerance = std::min(_settings.populationTolerance, _settings.maximumOnline - _settings.minimumOnline);
        _settings.loginRatePerMinute = std::clamp<std::uint32_t>(
            _settings.loginRatePerMinute, 1, SafetyLimits::MaximumLoginRatePerMinute);
        _settings.logoutRatePerMinute = std::clamp<std::uint32_t>(
            _settings.logoutRatePerMinute, 1, SafetyLimits::MaximumLogoutRatePerMinute);

        if (static_cast<std::uint8_t>(_settings.populationMode) > static_cast<std::uint8_t>(PopulationMode::Adaptive))
            _settings.populationMode = PopulationMode::Fixed;

        if (static_cast<std::uint8_t>(_settings.seedState) > static_cast<std::uint8_t>(SeedState::Custom))
            _settings.seedState = SeedState::Uninitialized;
    }
}
