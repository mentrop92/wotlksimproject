#ifndef MOD_LIVINGWORLD_STATE_ROLLBACK_H
#define MOD_LIVINGWORLD_STATE_ROLLBACK_H

#include "LivingWorldStateCheckpoint.h"

#include <cstdint>

namespace LivingWorld
{
    namespace StateRollbackLimits
    {
        inline constexpr std::uint64_t MaximumRollbackMinutes = 10080; // seven days
        inline constexpr std::uint32_t MaximumPopulationDelta = 2500;
    }

    enum class StateRollbackRejection : std::uint8_t
    {
        None = 0,
        IncompatibleCheckpoint,
        FutureCheckpoint,
        ExcessiveTimeRewind,
        ExcessivePopulationDelta,
        ExcessiveTargetDelta,
        ExcessiveQueuedOperationDelta
    };

    struct StateRollbackValidation
    {
        bool allowed = false;
        StateRollbackRejection rejection = StateRollbackRejection::IncompatibleCheckpoint;
        std::uint64_t rollbackMinutes = 0;
        std::uint32_t populationDelta = 0;
        std::uint32_t targetDelta = 0;
        std::uint32_t queuedOperationDelta = 0;
    };

    class StateRollbackValidator
    {
    public:
        static StateRollbackValidation Validate(
            StateCheckpoint const& current,
            StateCheckpoint const& target);
    };
}

#endif
