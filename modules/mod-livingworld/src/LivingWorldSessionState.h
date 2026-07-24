#ifndef MOD_LIVINGWORLD_SESSION_STATE_H
#define MOD_LIVINGWORLD_SESSION_STATE_H

#include <cstdint>

namespace LivingWorld
{
    struct SessionSignals
    {
        std::uint32_t elapsedMinutes = 0;
        std::uint32_t plannedMinutes = 0;
        bool activeGameplay = false;
        bool inCombat = false;
        bool socialActivity = false;
        bool meaningfulSuccess = false;
        bool meaningfulFailure = false;
    };

    struct SessionState
    {
        std::uint8_t motivation = 70;
        std::uint8_t fatigue = 0;
        std::uint8_t frustration = 0;
        std::uint8_t burnout = 0;
        std::uint8_t logoutReadiness = 0;
    };

    class SessionStateModel
    {
    public:
        static SessionState Update(
            SessionState const& previous,
            SessionSignals const& signals,
            std::uint8_t burnoutSensitivity,
            std::uint8_t patience);
    };
}

#endif
