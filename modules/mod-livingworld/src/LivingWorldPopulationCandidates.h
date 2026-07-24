#ifndef MOD_LIVINGWORLD_POPULATION_CANDIDATES_H
#define MOD_LIVINGWORLD_POPULATION_CANDIDATES_H

#include <cstdint>
#include <vector>

namespace LivingWorld
{
    struct PopulationCandidate
    {
        std::uint32_t characterGuid = 0;
        bool online = false;
        bool scheduleDue = false;
        bool sessionComplete = false;
        bool inCombat = false;
        bool inInstance = false;
        bool grouped = false;
        bool groupLeader = false;
        bool interactingWithHuman = false;
        bool protectedActivity = false;
        std::uint32_t minutesUntilSchedule = 0;
        std::uint32_t sessionOverrunMinutes = 0;
        std::uint8_t logoutReadiness = 0;
    };

    struct RankedCandidate
    {
        std::uint32_t characterGuid = 0;
        std::int32_t score = 0;
    };

    class PopulationCandidateSelector
    {
    public:
        static std::vector<RankedCandidate> RankLoginCandidates(
            std::vector<PopulationCandidate> const& candidates,
            std::uint32_t limit);
        static std::vector<RankedCandidate> RankLogoutCandidates(
            std::vector<PopulationCandidate> const& candidates,
            std::uint32_t limit,
            bool emergencyDrain);
    };
}

#endif
