#include "LivingWorldPopulationCandidates.h"

#include <cassert>
#include <iostream>
#include <vector>

using namespace LivingWorld;

int main()
{
    std::vector<PopulationCandidate> candidates = {
        { 1, false, true, false, false, false, false, false, false, false, 0, 0, 0, 0, 0, 0 },
        { 2, false, false, false, false, false, true, false, false, false, 30, 0, 0, 0, 0, 0 },
        { 3, true, false, true, false, false, false, false, false, false, 0, 45, 80, 0, 0, 0 },
        { 4, true, false, true, true, false, false, false, false, false, 0, 120, 100, 0, 0, 0 },
        { 5, true, false, true, false, true, true, true, true, false, 0, 180, 90, 0, 0, 0 },
        { 6, true, false, false, false, false, false, false, false, false, 0, 0, 20, 0, 0, 0 },
        { 7, true, false, true, false, false, false, false, false, true, 0, 300, 100, 0, 0, 0 },
        { 8, true, false, false, false, false, false, false, false, false, 0, 0, 95, 0, 0, 0 },
        { 9, false, false, false, false, false, false, false, false, false, 60, 0, 0, 500, 0, 0 },
        { 10, false, false, false, false, false, false, false, false, false, 0, 0, 0, 5000, 0, 0 },
        { 11, false, false, false, false, false, false, false, false, false, 0, 0, 0, 0, 300, 0 },
        { 12, false, false, false, false, false, false, false, false, false, 0, 0, 0, 0, 5000, 0 },
        { 13, false, false, false, false, false, false, false, false, false, 0, 0, 0, 0, 0, 250 },
        { 14, false, false, false, false, false, false, false, false, false, 0, 0, 0, 0, 0, 5000 }
    };

    auto login = PopulationCandidateSelector::RankLoginCandidates(candidates, 8);
    assert(login.size() == 8);
    assert(login[0].characterGuid == 1);
    assert(login[1].characterGuid == 10); // unsafe adherence values are clamped to +500
    assert(login[2].characterGuid == 9);
    assert(login[3].characterGuid == 11); // bounded faction-balancing adjustment
    assert(login[4].characterGuid == 12); // unsafe balance values are clamped to +300
    assert(login[5].characterGuid == 13); // bounded role-balancing adjustment
    assert(login[6].characterGuid == 14); // unsafe role values are clamped to +250
    assert(login[7].characterGuid == 2);

    auto logout = PopulationCandidateSelector::RankLogoutCandidates(candidates, 10, false);
    assert(logout.size() == 3);
    assert(logout[0].characterGuid == 3);
    assert(logout[1].characterGuid == 8);
    assert(logout[2].characterGuid == 6);

    auto emergency = PopulationCandidateSelector::RankLogoutCandidates(candidates, 10, true);
    assert(emergency.size() == 6);
    assert(emergency[0].characterGuid == 3);
    assert(emergency.back().characterGuid == 7);

    auto limited = PopulationCandidateSelector::RankLogoutCandidates(candidates, 1, false);
    assert(limited.size() == 1);
    assert(limited[0].characterGuid == 3);

    std::cout << "LivingWorld population candidate tests passed.\n";
    return 0;
}
