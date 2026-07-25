#include "LivingWorldStateRollback.h"

#include <algorithm>

namespace LivingWorld
{
    namespace
    {
        std::uint32_t AbsoluteDelta(std::uint32_t left, std::uint32_t right)
        {
            return left > right ? left - right : right - left;
        }
    }

    StateRollbackValidation StateRollbackValidator::Validate(
        StateCheckpoint const& current,
        StateCheckpoint const& target)
    {
        StateRollbackValidation validation;

        if (!StateCheckpointBuilder::IsCompatible(current) ||
            !StateCheckpointBuilder::IsCompatible(target))
        {
            validation.rejection = StateRollbackRejection::IncompatibleCheckpoint;
            return validation;
        }

        if (target.simulationMinute > current.simulationMinute)
        {
            validation.rejection = StateRollbackRejection::FutureCheckpoint;
            return validation;
        }

        validation.rollbackMinutes = current.simulationMinute - target.simulationMinute;
        validation.populationDelta = AbsoluteDelta(current.onlineAI, target.onlineAI);
        validation.targetDelta = AbsoluteDelta(current.effectiveTarget, target.effectiveTarget);
        validation.queuedOperationDelta = AbsoluteDelta(current.queuedOperations, target.queuedOperations);

        if (validation.rollbackMinutes > StateRollbackLimits::MaximumRollbackMinutes)
        {
            validation.rejection = StateRollbackRejection::ExcessiveTimeRewind;
            return validation;
        }

        if (validation.populationDelta > StateRollbackLimits::MaximumPopulationDelta)
        {
            validation.rejection = StateRollbackRejection::ExcessivePopulationDelta;
            return validation;
        }

        if (validation.targetDelta > StateRollbackLimits::MaximumPopulationDelta)
        {
            validation.rejection = StateRollbackRejection::ExcessiveTargetDelta;
            return validation;
        }

        if (validation.queuedOperationDelta > StateRollbackLimits::MaximumPopulationDelta)
        {
            validation.rejection = StateRollbackRejection::ExcessiveQueuedOperationDelta;
            return validation;
        }

        validation.allowed = true;
        validation.rejection = StateRollbackRejection::None;
        return validation;
    }
}
