#ifndef MOD_LIVINGWORLD_POPULATION_PLAN_H
#define MOD_LIVINGWORLD_POPULATION_PLAN_H

#include "LivingWorldPopulationCandidates.h"
#include "LivingWorldPopulationController.h"

#include <cstdint>
#include <vector>

namespace LivingWorld
{
    struct PopulationExecutionPlan
    {
        PopulationDecision decision;
        std::vector<RankedCandidate> selectedCandidates;
        bool emergencyDrain = false;
    };

    class PopulationPlanBuilder
    {
    public:
        static PopulationExecutionPlan Build(
            PopulationDecision const& decision,
            std::vector<PopulationCandidate> const& candidates,
            bool emergencyDrain = false);
    };
}

#endif
