#ifndef MOD_LIVINGWORLD_SETTINGS_H
#define MOD_LIVINGWORLD_SETTINGS_H

#include <cstdint>

namespace LivingWorld
{
    enum class PopulationMode : std::uint8_t
    {
        Fixed = 0,
        Scheduled,
        Adaptive
    };

    enum class SeedState : std::uint8_t
    {
        Uninitialized = 0,
        Fresh,
        Young,
        Established,
        Mature,
        Custom
    };

    struct WorldSettings
    {
        bool simulationEnabled = true;
        bool loginEnabled = true;
        std::uint32_t minimumOnline = 50;
        std::uint32_t maximumOnline = 500;
        std::uint32_t targetOnline = 250;
        bool autoScale = true;
        std::uint32_t loginRatePerMinute = 15;
        std::uint32_t logoutRatePerMinute = 20;
        PopulationMode populationMode = PopulationMode::Fixed;
        SeedState seedState = SeedState::Uninitialized;
    };

    class WorldSettingsManager
    {
    public:
        static WorldSettingsManager& Instance();

        void Load();
        WorldSettings const& Get() const;

        void SetOperationalState(bool simulationEnabled, bool loginEnabled);
        bool SetMinimumOnline(std::uint32_t value);
        bool SetMaximumOnline(std::uint32_t value);
        bool SetTargetOnline(std::uint32_t value);
        void SetAutoScale(bool enabled);

    private:
        void LoadDefaultsFromConfig();
        void Persist() const;
        void Normalize();

        WorldSettings _settings;
    };
}

#define sLivingWorldSettings LivingWorld::WorldSettingsManager::Instance()

#endif
