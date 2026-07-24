#include "LivingWorldSessionState.h"

#include <algorithm>
#include <cstdint>

namespace LivingWorld
{
    namespace
    {
        std::uint8_t ClampScore(std::int32_t value)
        {
            return static_cast<std::uint8_t>(std::clamp<std::int32_t>(value, 0, 100));
        }
    }

    SessionState SessionStateModel::Update(
        SessionState const& previous,
        SessionSignals const& signals,
        std::uint8_t burnoutSensitivity,
        std::uint8_t patience)
    {
        SessionState next = previous;

        std::int32_t fatigueDelta = signals.activeGameplay ? 3 : 1;
        if (signals.inCombat)
            fatigueDelta += 2;
        if (signals.elapsedMinutes > signals.plannedMinutes && signals.plannedMinutes > 0)
            fatigueDelta += 4;

        std::int32_t frustrationDelta = 0;
        if (signals.meaningfulFailure)
            frustrationDelta += std::max<std::int32_t>(2, 8 - patience / 20);
        if (signals.meaningfulSuccess)
            frustrationDelta -= 5;
        if (signals.socialActivity)
            frustrationDelta -= 2;

        std::int32_t motivationDelta = 0;
        if (signals.meaningfulSuccess)
            motivationDelta += 6;
        if (signals.meaningfulFailure)
            motivationDelta -= 4;
        if (signals.socialActivity)
            motivationDelta += 2;
        if (signals.elapsedMinutes > signals.plannedMinutes && signals.plannedMinutes > 0)
            motivationDelta -= 3;

        next.fatigue = ClampScore(static_cast<std::int32_t>(previous.fatigue) + fatigueDelta);
        next.frustration = ClampScore(static_cast<std::int32_t>(previous.frustration) + frustrationDelta);
        next.motivation = ClampScore(static_cast<std::int32_t>(previous.motivation) + motivationDelta);

        std::int32_t burnoutDelta = 0;
        if (next.fatigue >= 75)
            burnoutDelta += 1 + burnoutSensitivity / 35;
        if (next.frustration >= 70)
            burnoutDelta += 1;
        if (signals.meaningfulSuccess && next.fatigue < 60)
            burnoutDelta -= 1;
        next.burnout = ClampScore(static_cast<std::int32_t>(previous.burnout) + burnoutDelta);

        std::int32_t readiness = 0;
        readiness += next.fatigue / 2;
        readiness += next.frustration / 3;
        readiness += next.burnout / 3;
        readiness += (100 - next.motivation) / 3;

        if (signals.plannedMinutes > 0 && signals.elapsedMinutes >= signals.plannedMinutes)
            readiness += 20;
        if (signals.socialActivity)
            readiness -= 10;

        // Combat is a hard safety condition, not merely another weighted signal.
        // Even a fully exhausted bot should not become a normal logout candidate
        // while actively fighting. Emergency population handling may still rank it
        // separately after safer candidates have been exhausted.
        if (signals.inCombat)
            readiness = std::min<std::int32_t>(readiness, 25);

        next.logoutReadiness = ClampScore(readiness);
        return next;
    }
}
