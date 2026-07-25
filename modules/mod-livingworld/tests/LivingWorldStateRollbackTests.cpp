#include "LivingWorldStateRollback.h"

#include <cassert>
#include <iostream>

using namespace LivingWorld;

namespace
{
    StateCheckpoint MakeCheckpoint(
        std::uint64_t minute,
        std::uint32_t online,
        std::uint32_t target,
        std::uint32_t queued)
    {
        StateCheckpointInput input;
        input.simulationMinute = minute;
        input.onlineAI = online;
        input.effectiveTarget = target;
        input.queuedOperations = queued;
        input.label = "rollback-test";
        return StateCheckpointBuilder::Build(input);
    }
}

int main()
{
    StateCheckpoint const current = MakeCheckpoint(20000, 500, 550, 25);
    StateCheckpoint const safeTarget = MakeCheckpoint(19000, 450, 500, 10);

    StateRollbackValidation validation = StateRollbackValidator::Validate(current, safeTarget);
    assert(validation.allowed);
    assert(validation.rejection == StateRollbackRejection::None);
    assert(validation.rollbackMinutes == 1000);
    assert(validation.populationDelta == 50);
    assert(validation.targetDelta == 50);
    assert(validation.queuedOperationDelta == 15);

    StateCheckpoint future = safeTarget;
    future.simulationMinute = 21000;
    validation = StateRollbackValidator::Validate(current, future);
    assert(!validation.allowed);
    assert(validation.rejection == StateRollbackRejection::FutureCheckpoint);

    StateCheckpoint old = safeTarget;
    old.simulationMinute = current.simulationMinute - StateRollbackLimits::MaximumRollbackMinutes - 1;
    validation = StateRollbackValidator::Validate(current, old);
    assert(!validation.allowed);
    assert(validation.rejection == StateRollbackRejection::ExcessiveTimeRewind);

    StateCheckpoint populationJump = safeTarget;
    populationJump.onlineAI = current.onlineAI + StateRollbackLimits::MaximumPopulationDelta + 1;
    validation = StateRollbackValidator::Validate(current, populationJump);
    assert(!validation.allowed);
    assert(validation.rejection == StateRollbackRejection::ExcessivePopulationDelta);

    StateCheckpoint targetJump = safeTarget;
    targetJump.effectiveTarget = current.effectiveTarget + StateRollbackLimits::MaximumPopulationDelta + 1;
    validation = StateRollbackValidator::Validate(current, targetJump);
    assert(!validation.allowed);
    assert(validation.rejection == StateRollbackRejection::ExcessiveTargetDelta);

    StateCheckpoint queueJump = safeTarget;
    queueJump.queuedOperations = current.queuedOperations + StateRollbackLimits::MaximumPopulationDelta + 1;
    validation = StateRollbackValidator::Validate(current, queueJump);
    assert(!validation.allowed);
    assert(validation.rejection == StateRollbackRejection::ExcessiveQueuedOperationDelta);

    StateCheckpoint incompatible = safeTarget;
    incompatible.schemaVersion = StateCheckpointLimits::CurrentSchemaVersion + 1;
    validation = StateRollbackValidator::Validate(current, incompatible);
    assert(!validation.allowed);
    assert(validation.rejection == StateRollbackRejection::IncompatibleCheckpoint);

    std::cout << "LivingWorld state rollback validation tests passed.\n";
    return 0;
}
