#include "LivingWorldMemorySocialGraph.h"

#include <algorithm>
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
    memory.valenceBasisPoints = 3500;
    memory.decayBasisPointsPerDay = 250;
    memory.kind = MemoryKind::Cooperation;
    memory.tag = "dungeon_clear";
    assert(graph.AddMemory(memory));
    assert(graph.Memories().size() == 1);

    auto effective = LivingWorldMemorySocialGraph::EffectiveSalienceBasisPoints(memory, 100);
    assert(effective.has_value() && *effective == 7500);
    effective = LivingWorldMemorySocialGraph::EffectiveSalienceBasisPoints(
        memory, 100 + 4 * LivingWorldMemorySocialGraph::SimulationMinutesPerDay);
    assert(effective.has_value() && *effective == 6500);
    assert(!LivingWorldMemorySocialGraph::EffectiveSalienceBasisPoints(memory, 99).has_value());
    assert(LivingWorldMemorySocialGraph::IsMeaningfulAt(memory, 100));

    LivingWorldMemoryRecord fading = memory;
    fading.simulationMinute = 200;
    fading.salienceBasisPoints = 500;
    fading.decayBasisPointsPerDay = 200;
    effective = LivingWorldMemorySocialGraph::EffectiveSalienceBasisPoints(
        fading, 200 + 2 * LivingWorldMemorySocialGraph::SimulationMinutesPerDay);
    assert(effective.has_value() && *effective == 100);
    assert(LivingWorldMemorySocialGraph::IsMeaningfulAt(
        fading, 200 + 2 * LivingWorldMemorySocialGraph::SimulationMinutesPerDay));
    effective = LivingWorldMemorySocialGraph::EffectiveSalienceBasisPoints(
        fading, 200 + 3 * LivingWorldMemorySocialGraph::SimulationMinutesPerDay);
    assert(effective.has_value() && *effective == 0);
    assert(!LivingWorldMemorySocialGraph::IsMeaningfulAt(
        fading, 200 + 3 * LivingWorldMemorySocialGraph::SimulationMinutesPerDay));

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
    memory.salienceBasisPoints = LivingWorldMemorySocialGraph::MinimumMeaningfulSalienceBasisPoints - 1;
    assert(!graph.AddMemory(memory));
    memory.salienceBasisPoints = 7500;
    memory.valenceBasisPoints = 10001;
    assert(!graph.AddMemory(memory));
    memory.valenceBasisPoints = 3500;
    memory.decayBasisPointsPerDay = LivingWorldMemorySocialGraph::MaximumDecayBasisPointsPerDay + 1;
    assert(!graph.AddMemory(memory));
    memory.decayBasisPointsPerDay = 250;
    memory.kind = static_cast<MemoryKind>(255);
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
        item.salienceBasisPoints = LivingWorldMemorySocialGraph::MinimumMeaningfulSalienceBasisPoints;
        item.kind = MemoryKind::Encounter;
        item.tag = "encounter";
        assert(graph.AddMemory(item));
    }
    assert(graph.Memories().size() == LivingWorldMemorySocialGraph::MaximumMemories);
    assert(graph.Memories().front().simulationMinute == 5);

    graph.Reset();
    LivingWorldMemoryRecord protectedMemory;
    protectedMemory.actorId = 30;
    protectedMemory.subjectId = 40;
    protectedMemory.salienceBasisPoints = 1000;
    protectedMemory.decayBasisPointsPerDay = LivingWorldMemorySocialGraph::MaximumDecayBasisPointsPerDay;
    protectedMemory.protectedMemory = true;
    protectedMemory.kind = MemoryKind::Organization;
    protectedMemory.tag = "guild_founding";

    for (std::size_t i = 0; i < LivingWorldMemorySocialGraph::MaximumProtectedMemories; ++i)
    {
        protectedMemory.simulationMinute = i;
        protectedMemory.subjectId = 40 + i;
        assert(graph.AddMemory(protectedMemory));
    }
    effective = LivingWorldMemorySocialGraph::EffectiveSalienceBasisPoints(
        graph.Memories().front(), 100 * LivingWorldMemorySocialGraph::SimulationMinutesPerDay);
    assert(effective.has_value() && *effective == 1000);

    protectedMemory.simulationMinute = LivingWorldMemorySocialGraph::MaximumProtectedMemories;
    protectedMemory.subjectId = 1000;
    assert(!graph.AddMemory(protectedMemory));

    std::uint64_t nextMinute = 1000;
    while (graph.Memories().size() < LivingWorldMemorySocialGraph::MaximumMemories)
    {
        LivingWorldMemoryRecord item;
        item.actorId = 50;
        item.subjectId = 10000 + nextMinute;
        item.simulationMinute = nextMinute++;
        item.salienceBasisPoints = 500;
        item.tag = "ordinary_event";
        assert(graph.AddMemory(item));
    }

    LivingWorldMemoryRecord newcomer;
    newcomer.actorId = 60;
    newcomer.subjectId = 61;
    newcomer.simulationMinute = nextMinute;
    newcomer.salienceBasisPoints = 500;
    newcomer.tag = "new_event";
    assert(graph.AddMemory(newcomer));
    assert(graph.Memories().size() == LivingWorldMemorySocialGraph::MaximumMemories);
    assert(std::count_if(graph.Memories().begin(), graph.Memories().end(), [](LivingWorldMemoryRecord const& item)
    {
        return item.protectedMemory;
    }) == static_cast<std::ptrdiff_t>(LivingWorldMemorySocialGraph::MaximumProtectedMemories));
    assert(graph.Memories().front().protectedMemory);
    assert(graph.Memories().front().simulationMinute == 0);
    assert(std::none_of(graph.Memories().begin(), graph.Memories().end(), [](LivingWorldMemoryRecord const& item)
    {
        return !item.protectedMemory && item.simulationMinute == 1000;
    }));

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

    std::cout << "LivingWorld bounded memory/social graph lifecycle tests passed.\n";
    return 0;
}
