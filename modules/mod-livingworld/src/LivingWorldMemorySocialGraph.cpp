#include "LivingWorldMemorySocialGraph.h"

#include <algorithm>
#include <cctype>

namespace LivingWorld
{
bool LivingWorldMemorySocialGraph::IsSafeTag(std::string const& tag)
{
    if (tag.empty() || tag.size() > MaximumTagBytes)
        return false;

    for (unsigned char c : tag)
    {
        if (!(std::isalnum(c) || c == '-' || c == '_' || c == '.'))
            return false;
    }
    return true;
}

bool LivingWorldMemorySocialGraph::IsValidMemoryKind(MemoryKind kind)
{
    switch (kind)
    {
        case MemoryKind::Encounter:
        case MemoryKind::Cooperation:
        case MemoryKind::Conflict:
        case MemoryKind::Trade:
        case MemoryKind::Organization:
            return true;
    }
    return false;
}

bool LivingWorldMemorySocialGraph::IsValidMemory(LivingWorldMemoryRecord const& record)
{
    if (record.actorId == 0 || record.subjectId == 0)
        return false;
    if (record.salienceBasisPoints < MinimumMeaningfulSalienceBasisPoints ||
        record.salienceBasisPoints > MaximumUnsignedBasisPoints)
        return false;
    if (record.valenceBasisPoints < MinimumSignedBasisPoints || record.valenceBasisPoints > MaximumSignedBasisPoints)
        return false;
    if (record.decayBasisPointsPerDay > MaximumDecayBasisPointsPerDay)
        return false;
    if (!IsValidMemoryKind(record.kind))
        return false;
    return IsSafeTag(record.tag);
}

bool LivingWorldMemorySocialGraph::IsValidRelation(LivingWorldSocialEdge const& edge)
{
    if (edge.actorId == 0 || edge.otherId == 0 || edge.actorId == edge.otherId)
        return false;
    if (edge.affinityBasisPoints < MinimumSignedBasisPoints || edge.affinityBasisPoints > MaximumSignedBasisPoints)
        return false;
    if (edge.trustBasisPoints > MaximumUnsignedBasisPoints || edge.familiarityBasisPoints > MaximumUnsignedBasisPoints)
        return false;
    return true;
}

bool LivingWorldMemorySocialGraph::AddMemory(LivingWorldMemoryRecord record)
{
    if (!IsValidMemory(record))
        return false;
    if (!memories_.empty() && record.simulationMinute < memories_.back().simulationMinute)
        return false;

    if (record.protectedMemory)
    {
        std::size_t protectedCount = static_cast<std::size_t>(std::count_if(
            memories_.begin(), memories_.end(), [](LivingWorldMemoryRecord const& candidate)
            {
                return candidate.protectedMemory;
            }));
        if (protectedCount >= MaximumProtectedMemories)
            return false;
    }

    if (memories_.size() >= MaximumMemories)
    {
        auto evict = std::find_if(memories_.begin(), memories_.end(), [](LivingWorldMemoryRecord const& candidate)
        {
            return !candidate.protectedMemory;
        });
        if (evict == memories_.end())
            return false;
        memories_.erase(evict);
    }

    memories_.push_back(std::move(record));
    return true;
}

bool LivingWorldMemorySocialGraph::UpsertRelation(LivingWorldSocialEdge edge)
{
    if (!IsValidRelation(edge))
        return false;

    auto existing = std::find_if(relations_.begin(), relations_.end(), [&](LivingWorldSocialEdge const& candidate)
    {
        return candidate.actorId == edge.actorId && candidate.otherId == edge.otherId;
    });

    if (existing != relations_.end())
    {
        if (edge.lastInteractionSimulationMinute < existing->lastInteractionSimulationMinute)
            return false;
        *existing = edge;
        return true;
    }

    relations_.push_back(edge);
    if (relations_.size() > MaximumRelations)
        relations_.erase(relations_.begin());
    return true;
}

std::vector<LivingWorldMemoryRecord> const& LivingWorldMemorySocialGraph::Memories() const
{
    return memories_;
}

std::vector<LivingWorldSocialEdge> const& LivingWorldMemorySocialGraph::Relations() const
{
    return relations_;
}

std::optional<LivingWorldSocialEdge> LivingWorldMemorySocialGraph::FindRelation(std::uint64_t actorId, std::uint64_t otherId) const
{
    auto found = std::find_if(relations_.begin(), relations_.end(), [&](LivingWorldSocialEdge const& candidate)
    {
        return candidate.actorId == actorId && candidate.otherId == otherId;
    });
    if (found == relations_.end())
        return std::nullopt;
    return *found;
}

std::optional<std::uint16_t> LivingWorldMemorySocialGraph::EffectiveSalienceBasisPoints(
    LivingWorldMemoryRecord const& record,
    std::uint64_t atSimulationMinute)
{
    if (!IsValidMemory(record) || atSimulationMinute < record.simulationMinute)
        return std::nullopt;
    if (record.protectedMemory || record.decayBasisPointsPerDay == 0)
        return record.salienceBasisPoints;

    std::uint64_t elapsedDays = (atSimulationMinute - record.simulationMinute) / SimulationMinutesPerDay;
    if (elapsedDays == 0)
        return record.salienceBasisPoints;

    std::uint64_t daysToZero =
        (static_cast<std::uint64_t>(record.salienceBasisPoints) + record.decayBasisPointsPerDay - 1) /
        record.decayBasisPointsPerDay;
    if (elapsedDays >= daysToZero)
        return 0;

    std::uint64_t decayed = elapsedDays * record.decayBasisPointsPerDay;
    return static_cast<std::uint16_t>(record.salienceBasisPoints - decayed);
}

bool LivingWorldMemorySocialGraph::IsMeaningfulAt(
    LivingWorldMemoryRecord const& record,
    std::uint64_t atSimulationMinute)
{
    auto effective = EffectiveSalienceBasisPoints(record, atSimulationMinute);
    return effective.has_value() && *effective >= MinimumMeaningfulSalienceBasisPoints;
}

void LivingWorldMemorySocialGraph::Reset()
{
    memories_.clear();
    relations_.clear();
}
}
