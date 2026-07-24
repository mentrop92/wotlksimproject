#ifndef MOD_LIVINGWORLD_POPULATION_DIAGNOSTICS_H
#define MOD_LIVINGWORLD_POPULATION_DIAGNOSTICS_H

#include "LivingWorldPopulationCandidates.h"

#include <cstdint>
#include <vector>

namespace LivingWorld
{
    struct PopulationDiagnostics
    {
        std::uint32_t totalCandidates = 0;
        std::uint32_t onlineCandidates = 0;
        std::uint32_t offlineCandidates = 0;
        std::uint32_t loginReady = 0;
        std::uint32_t normalLogoutReady = 0;
        std::uint32_t emergencyOnly = 0;
        std::uint32_t blockedByCombat = 0;
        std::uint32_t blockedByInstance = 0;
        std::uint32_t blockedByLeadership = 0;
        std::uint32_t blockedByHumanInteraction = 0;
        std::uint32_t blockedByProtectedActivity = 0;
        std::uint32_t blockedByCooldown = 0;
    };

    class PopulationDiagnosticsBuilder
    {
    public:
        static PopulationDiagnostics Analyze(std::vector<PopulationCandidate> const& candidates);
    };
}

#endif
