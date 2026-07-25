#include "LivingWorldPopulationPolicy.h"

#include <algorithm>

namespace LivingWorld
{
    PopulationPolicyResult PopulationPolicy::Evaluate(
        PopulationPolicyInput const& input,
        WorldSettings const& settings,
        ServerLoadPolicy const& loadPolicy)
    {
        PopulationPolicyResult result;
        result.configuredTarget = settings.targetOnline;
        result.scheduledTarget = settings.targetOnline;

        WorldSettings effectiveSettings = settings;

        if (settings.populationMode == PopulationMode::Scheduled || settings.populationMode == PopulationMode::Adaptive)
        {
            result.curve = PopulationCurve::Evaluate({
                settings.minimumOnline,
                settings.targetOnline,
                settings.maximumOnline,
                input.weekday,
                input.minuteOfDay
            });
            result.scheduledTarget = result.curve.target;
            result.scheduleApplied = true;
        }
        else
        {
            result.curve.target = settings.targetOnline;
        }

        result.loadDecision.requestedTarget = result.scheduledTarget;
        result.loadDecision.governedTarget = result.scheduledTarget;

        if (settings.populationMode == PopulationMode::Adaptive)
        {
            result.loadDecision = LoadGovernor::Evaluate(result.scheduledTarget, input.load, loadPolicy);
            result.loadApplied = result.loadDecision.constrained;
            result.emergencyDrain = result.loadDecision.emergencyDrain;
        }

        result.governedTarget = std::clamp(
            result.loadDecision.governedTarget,
            0U,
            std::min(settings.maximumOnline, SafetyLimits::MaximumPopulation));
        effectiveSettings.targetOnline = result.governedTarget;

        // If load pressure pushes the target below the configured minimum, the
        // effective minimum must yield as well. This affects only the dry-run
        // decision copy and never mutates persisted administrator settings.
        effectiveSettings.minimumOnline = std::min(effectiveSettings.minimumOnline, effectiveSettings.targetOnline);

        result.populationDecision = sLivingWorldPopulation.Update(
            input.diffMs,
            input.currentAIOnline,
            input.currentHumanOnline,
            effectiveSettings);

        return result;
    }
}
