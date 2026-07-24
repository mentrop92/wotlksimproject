#ifndef MOD_LIVINGWORLD_SETTINGS_H
#define MOD_LIVINGWORLD_SETTINGS_H

#include <cstdint>

namespace LivingWorld
{
    namespace SafetyLimits
    {
        inline constexpr std::uint32_t MaximumPopulation = 10000;
        inline constexpr std::uint32_t MaximumTotalOnline = 20000;
        inline constexpr std::uint32_t MaximumLoginRatePerMinute = 100;
        inline constexpr std::uint32_t MaximumLogoutRatePerMinute = 200;
    }

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
        std::uint32_t populationTolerance = 15;
        bool autoScale = true;
        std::uint32_t loginRatePerMinute = 15;
        std::uint32_t logoutRatePerMinute = 20;

        // Phase 2 capacity policy. The controller always reserves at least this
        // many total slots for humans, and yields additional slots when the
        // observed human population exceeds the reserve.
        std::uint32_t maximumTotalOnline = 600;
        std::uint32_t reservedHumanSlots = 100;

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
        bool SetPopulationTolerance(std::uint32_t value);
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
