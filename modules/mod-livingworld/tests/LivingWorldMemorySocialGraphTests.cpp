#include "LivingWorldMemorySocialGraph.h"

#include <cassert>
#include <iostream>
#include <string>

using namespace LivingWorld;

int main()
{
    LivingWorldMemorySocialGraph graph;

    LivingWorldMemoryRecord memory;
    memory.actorId = 1;
    memory.subjectId = 2;
    memory.simulationMinute = 100;
    memory.salienceBasisPoints = 7500;
    memory.kind = MemoryKind::Cooperation;
    memory.tag = "dungeon_clear";
    assert(graph.AddMemory(memory));
    assert(graph.Memories().size() == 1);

    memory.simulationMinute = 99;
    assert(!graph.AddMemory(memory));
    memory.simulationMinute = 101;
    memory.tag = "unsafe tag";
    assert(!graph.AddMemory(memory));
    memory.tag = std::string(LivingWorldMemorySocialGraph::MaximumTagBytes + 1, 'a');
    assert(!graph.AddMemory(memory));
    memory.tag = "trade.win-01";
    memory.salienceBasisPoints = 10001;
    assert(!graph.AddMemory(memory));

    LivingWorldSocialEdge edge;
    edge.actorId = 1;
    edge.otherId = 2;
    edge.affinityBasisPoints = 2500;
    edge.trustBasisPoints = 4000;
    edge.familiarityBasisPoints = 3000;
    edge.lastInteractionSimulationMinute = 100;
    assert(graph.UpsertRelation(edge));
    assert(graph.Relations().size() == 1);

    auto found = graph.FindRelation(1, 2);
    assert(found.has_value());
    assert(found->affinityBasisPoints == 2500);

    edge.affinityBasisPoints = 3000;
    edge.lastInteractionSimulationMinute = 120;
    assert(graph.UpsertRelation(edge));
    assert(graph.Relations().size() == 1);
    found = graph.FindRelation(1, 2);
    assert(found.has_value() && found->affinityBasisPoints == 3000);

    edge.lastInteractionSimulationMinute = 119;
    assert(!graph.UpsertRelation(edge));
    edge.lastInteractionSimulationMinute = 121;
    edge.affinityBasisPoints = 10001;
    assert(!graph.UpsertRelation(edge));
    edge.affinityBasisPoints = 0;
    edge.otherId = 1;
    assert(!graph.UpsertRelation(edge));

    graph.Reset();
    assert(graph.Memories().empty());
    assert(graph.Relations().empty());

    for (std::size_t i = 0; i < LivingWorldMemorySocialGraph::MaximumMemories + 5; ++i)
    {
        LivingWorldMemoryRecord item;
        item.actorId = 10;
        item.subjectId = 20;
        item.simulationMinute = i;
        item.salienceBasisPoints = 100;
        item.kind = MemoryKind::Encounter;
        item.tag = "encounter";
        assert(graph.AddMemory(item));
    }
    assert(graph.Memories().size() == LivingWorldMemorySocialGraph::MaximumMemories);
    assert(graph.Memories().front().simulationMinute == 5);

    graph.Reset();
    for (std::size_t i = 0; i < LivingWorldMemorySocialGraph::MaximumRelations + 3; ++i)
    {
        LivingWorldSocialEdge relation;
        relation.actorId = 100;
        relation.otherId = 1000 + i;
        relation.lastInteractionSimulationMinute = i;
        assert(graph.UpsertRelation(relation));
    }
    assert(graph.Relations().size() == LivingWorldMemorySocialGraph::MaximumRelations);
    assert(graph.FindRelation(100, 1000) == std::nullopt);
    assert(graph.FindRelation(100, 1003).has_value());

    std::cout << "LivingWorld bounded memory/social graph tests passed.\n";
    return 0;
}
