#include "LivingWorldRelationshipPolicy.h"

#include <cassert>
#include <iostream>

using namespace LivingWorld;

int main()
{
    LivingWorldSocialEdge relation;
    relation.actorId = 1;
    relation.otherId = 2;
    relation.affinityBasisPoints = 1000;
    relation.trustBasisPoints = 1000;
    relation.familiarityBasisPoints = 750;
    relation.rivalryBasisPoints = 250;

    auto invitation = LivingWorldRelationshipPolicy::Evaluate(relation, RelationshipChoice::Invitation);
    assert(invitation.eligible);
    assert(invitation.scoreBasisPoints == 2500);
    assert(invitation.requiredScoreBasisPoints == 1500);

    auto assistance = LivingWorldRelationshipPolicy::Evaluate(relation, RelationshipChoice::Assistance);
    assert(assistance.eligible);

    auto trade = LivingWorldRelationshipPolicy::Evaluate(relation, RelationshipChoice::Trade);
    assert(trade.eligible);

    auto guild = LivingWorldRelationshipPolicy::Evaluate(relation, RelationshipChoice::Guild);
    assert(!guild.eligible);

    relation.affinityBasisPoints = 2500;
    relation.trustBasisPoints = 3000;
    relation.familiarityBasisPoints = 2500;
    relation.rivalryBasisPoints = 500;
    guild = LivingWorldRelationshipPolicy::Evaluate(relation, RelationshipChoice::Guild);
    assert(guild.eligible);
    assert(guild.scoreBasisPoints == 7500);

    relation.rivalryBasisPoints = 7000;
    invitation = LivingWorldRelationshipPolicy::Evaluate(relation, RelationshipChoice::Invitation);
    assert(!invitation.eligible);

    relation.rivalryBasisPoints = 6000;
    relation.trustBasisPoints = 0;
    relation.familiarityBasisPoints = 10000;
    relation.affinityBasisPoints = 10000;
    trade = LivingWorldRelationshipPolicy::Evaluate(relation, RelationshipChoice::Trade);
    assert(!trade.eligible);

    relation.rivalryBasisPoints = 0;
    relation.trustBasisPoints = 10000;
    relation.familiarityBasisPoints = 10000;
    relation.affinityBasisPoints = 10000;
    assistance = LivingWorldRelationshipPolicy::Evaluate(relation, RelationshipChoice::Assistance);
    assert(assistance.scoreBasisPoints == LivingWorldRelationshipPolicy::MaximumScoreBasisPoints);

    relation.trustBasisPoints = 0;
    relation.familiarityBasisPoints = 0;
    relation.affinityBasisPoints = -10000;
    relation.rivalryBasisPoints = 10000;
    assistance = LivingWorldRelationshipPolicy::Evaluate(relation, RelationshipChoice::Assistance);
    assert(!assistance.eligible);
    assert(assistance.scoreBasisPoints == LivingWorldRelationshipPolicy::MinimumScoreBasisPoints);

    relation.actorId = 0;
    invitation = LivingWorldRelationshipPolicy::Evaluate(relation, RelationshipChoice::Invitation);
    assert(!invitation.eligible);

    relation.actorId = 1;
    relation.otherId = 1;
    assistance = LivingWorldRelationshipPolicy::Evaluate(relation, RelationshipChoice::Assistance);
    assert(!assistance.eligible);

    relation.otherId = 2;
    auto invalid = LivingWorldRelationshipPolicy::Evaluate(relation, static_cast<RelationshipChoice>(255));
    assert(!invalid.eligible);
    assert(invalid.scoreBasisPoints == 0);
    assert(invalid.requiredScoreBasisPoints == 0);

    std::cout << "LivingWorld relationship decision policy tests passed.\n";
    return 0;
}
