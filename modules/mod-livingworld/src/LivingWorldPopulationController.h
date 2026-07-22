#ifndef MOD_LIVINGWORLD_POPULATION_CONTROLLER_H
#define MOD_LIVINGWORLD_POPULATION_CONTROLLER_H

#include "LivingWorldSettings.h"

#include <cstdint>

namespace LivingWorld
{
    enum class PopulationAction : std::uint8_t
    {
        None = 0,
        Login,
        Logout
    };

    struct PopulationDecision
    {
        PopulationAction action = PopulationAction::None;
        std::uint32_t requestedCount = 0;
        std::uint32_t currentOnline = 0;
        std::uint32_t effectiveTarget = 0;
        std::uint32_t lowerBound = 0;
        std::uint32_t upperBound = 0;
    };

    class PopulationController
    {
    public:
        static PopulationController& Instance();

        void Reset();
        PopulationDecision Update(std::uint32_t diffMs, std::uint32_t currentOnline, WorldSettings const& settings);
        PopulationDecision const& GetLastDecision() const;

    private:
        static std::uint32_t CalculateBudget(std::uint64_t& credit, std::uint32_t diffMs, std::uint32_t ratePerMinute);
        static std::uint32_t EffectiveTarget(std::uint32_t currentOnline, WorldSettings const& settings);
        static void CalculateBand(PopulationDecision& decision, WorldSettings const& settings);

        std::uint64_t _loginCredit = 0;
        std::uint64_t _logoutCredit = 0;
        PopulationDecision _lastDecision;
    };
}

#define sLivingWorldPopulation LivingWorld::PopulationController::Instance()

#endif
