#include "LivingWorldPopulationCandidates.h"

#include <cassert>
#include <iostream>
#include <vector>

using namespace LivingWorld;

int main()
{
    std::vector<PopulationCandidate> candidates = {
        { 1, false, true, false, false, false, false, false, false, false, 0, 0 },
        { 2, false, false, false, false, false, true, false, false, false, 30, 0 },
        { 3, true, false, true, false, false, false, false, false, false, 0, 45 },
        { 4, true, false, true, true, false, false, false, false, false, 0, 120 },
        { 5, true, false, true, false, true, true, true, true, false, 0, 180 },
        { 6, true, false, false, false, false, false, false, false, false, 0, 0 },
        { 7, true, false, true, false, false, false, false, false, true, 0, 300 }
    };

    auto login = PopulationCandidateSelector::RankLoginCandidates(candidates, 2);
    assert(login.size() == 2);
    assert(login[0].characterGuid == 1);
    assert(login[1].characterGuid == 2);

    auto logout = PopulationCandidateSelector::RankLogoutCandidates(candidates, 10, false);
    assert(logout.size() == 2);
    assert(logout[0].characterGuid == 3);
    assert(logout[1].characterGuid == 6);

    auto emergency = PopulationCandidateSelector::RankLogoutCandidates(candidates, 10, true);
    assert(emergency.size() == 5);
    assert(emergency[0].characterGuid == 3);
    assert(emergency.back().characterGuid == 7);

    auto limited = PopulationCandidateSelector::RankLogoutCandidates(candidates, 1, false);
    assert(limited.size() == 1);
    assert(limited[0].characterGuid == 3);

    std::cout << "LivingWorld population candidate tests passed.\n";
    return 0;
}
