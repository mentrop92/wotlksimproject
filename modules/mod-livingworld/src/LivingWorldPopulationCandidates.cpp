#include "LivingWorldPopulationCandidates.h"

#include <algorithm>

namespace LivingWorld
{
    namespace
    {
        std::vector<RankedCandidate> TakeBest(std::vector<RankedCandidate> ranked, std::uint32_t limit)
        {
            std::sort(ranked.begin(), ranked.end(), [](RankedCandidate const& left, RankedCandidate const& right)
            {
                if (left.score != right.score)
                    return left.score > right.score;
                return left.characterGuid < right.characterGuid;
            });

            if (ranked.size() > limit)
                ranked.resize(limit);
            return ranked;
        }
    }

    std::vector<RankedCandidate> PopulationCandidateSelector::RankLoginCandidates(
        std::vector<PopulationCandidate> const& candidates,
        std::uint32_t limit)
    {
        std::vector<RankedCandidate> ranked;
        for (PopulationCandidate const& candidate : candidates)
        {
            if (candidate.online || candidate.protectedActivity)
                continue;

            std::int32_t score = candidate.scheduleDue ? 1000 : 0;
            score -= static_cast<std::int32_t>(std::min<std::uint32_t>(candidate.minutesUntilSchedule, 720));
            if (candidate.grouped)
                score += 150;
            ranked.push_back({ candidate.characterGuid, score });
        }
        return TakeBest(std::move(ranked), limit);
    }

    std::vector<RankedCandidate> PopulationCandidateSelector::RankLogoutCandidates(
        std::vector<PopulationCandidate> const& candidates,
        std::uint32_t limit,
        bool emergencyDrain)
    {
        std::vector<RankedCandidate> ranked;
        for (PopulationCandidate const& candidate : candidates)
        {
            if (!candidate.online)
                continue;

            if (!emergencyDrain && (candidate.inCombat || candidate.inInstance || candidate.groupLeader ||
                candidate.interactingWithHuman || candidate.protectedActivity))
            {
                continue;
            }

            std::int32_t score = candidate.sessionComplete ? 1000 : 0;
            score += static_cast<std::int32_t>(std::min<std::uint32_t>(candidate.sessionOverrunMinutes, 720));
            if (candidate.grouped)
                score -= 250;
            if (candidate.inInstance)
                score -= 500;
            if (candidate.groupLeader)
                score -= 800;
            if (candidate.interactingWithHuman)
                score -= 1000;
            if (candidate.inCombat)
                score -= 2000;
            if (candidate.protectedActivity)
                score -= 3000;

            ranked.push_back({ candidate.characterGuid, score });
        }
        return TakeBest(std::move(ranked), limit);
    }
}
