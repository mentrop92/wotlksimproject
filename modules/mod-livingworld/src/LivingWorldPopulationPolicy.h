#ifndef MOD_LIVINGWORLD_POPULATION_POLICY_H
#define MOD_LIVINGWORLD_POPULATION_POLICY_H

#include "LivingWorldLoadGovernor.h"
#include "LivingWorldPopulationController.h"
#include "LivingWorldPopulationCurve.h"
#include "LivingWorldSettings.h"

#include <cstdint>

namespace LivingWorld
{
    struct PopulationPolicyInput
    {
        std::uint32_t diffMs = 0;
        std::uint32_t currentAIOnline = 0;
        std::uint32_t currentHumanOnline = 0;
        std::uint8_t weekday = 0;
        std::uint16_t minuteOfDay = 0;
        ServerLoadSnapshot load;
    };

    struct PopulationPolicyResult
    {
        std::uint32_t configuredTarget = 0;
        std::uint32_t scheduledTarget = 0;
        std::uint32_t governedTarget = 0;
        bool scheduleApplied = false;
        bool loadApplied = false;
        bool emergencyDrain = false;
        PopulationCurveResult curve;
        ServerLoadDecision loadDecision;
        PopulationDecision populationDecision;
    };

    class PopulationPolicy
    {
    public:
        static PopulationPolicyResult Evaluate(
            PopulationPolicyInput const& input,
            WorldSettings const& settings,
            ServerLoadPolicy const& loadPolicy = ServerLoadPolicy{});
    };
}

#endif
