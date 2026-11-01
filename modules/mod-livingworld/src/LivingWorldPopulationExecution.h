#ifndef MOD_LIVINGWORLD_POPULATION_EXECUTION_H
#define MOD_LIVINGWORLD_POPULATION_EXECUTION_H

#include "LivingWorldPopulationCandidates.h"
#include "LivingWorldPopulationController.h"

#include <cstdint>
#include <vector>

namespace LivingWorld
{
    // Playerbots owns random-bot login and logout. RandomPlayerbotMgr logs bots
    // in while its "bot_count" target exceeds the scheduled bot list, and logs
    // a bot out once that bot's "add" event is cleared and it is not grouped.
    // LivingWorld therefore steers those two levers instead of calling
    // AddPlayerBot/LogoutPlayerBot directly, which the manager would undo.
    struct PlayerbotPopulationLimits
    {
        // RandomPlayerbotMgr replaces any "bot_count" outside this range with
        // a random value, so every steered target is clamped into it.
        std::uint32_t minimumRandomBots = 0;
        std::uint32_t maximumRandomBots = 0;
    };

    struct PopulationExecutionPlan
    {
        bool writeBotCountTarget = false;
        std::uint32_t botCountTarget = 0;
        bool clampedToPlayerbotLimits = false;
        std::vector<std::uint32_t> logoutGuids;
    };

    class PopulationExecutionPlanner
    {
    public:
        // currentBotCountTarget is the manager's present "bot_count" value.
        // The plan only requests a write when the target changes, because
        // every write is a Playerbots database transaction.
        static PopulationExecutionPlan Plan(
            PopulationDecision const& decision,
            std::uint32_t onlineRandomBots,
            std::uint32_t currentBotCountTarget,
            PlayerbotPopulationLimits const& limits,
            std::vector<PopulationCandidate> const& logoutCandidates);
    };
}

#endif
