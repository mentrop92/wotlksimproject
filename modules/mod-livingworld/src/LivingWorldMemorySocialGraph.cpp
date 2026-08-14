#include "LivingWorldMemorySocialGraph.h"

#include <algorithm>
#include <cctype>
#include <cstdint>

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
    if (edge.trustBasisPoints > MaximumUnsignedBasisPoints ||
        edge.familiarityBasisPoints > MaximumUnsignedBasisPoints ||
        edge.rivalryBasisPoints > MaximumUnsignedBasisPoints)
        return false;
    return true;
}

bool LivingWorldMemorySocialGraph::IsValidSocialEvent(LivingWorldSocialEvent const& event)
{
    if (event.actorId == 0 || event.otherId == 0 || event.actorId == event.otherId)
        return false;

    auto validDelta = [](std::int16_t delta)
    {
        return delta >= -MaximumSocialEventDeltaBasisPoints && delta <= MaximumSocialEventDeltaBasisPoints;
    };

    if (!validDelta(event.affinityDeltaBasisPoints) ||
        !validDelta(event.trustDeltaBasisPoints) ||
        !validDelta(event.familiarityDeltaBasisPoints) ||
        !validDelta(event.rivalryDeltaBasisPoints))
        return false;

    return event.affinityDeltaBasisPoints != 0 ||
        event.trustDeltaBasisPoints != 0 ||
        event.familiarityDeltaBasisPoints != 0 ||
        event.rivalryDeltaBasisPoints != 0;
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

bool LivingWorldMemorySocialGraph::ApplySocialEvent(LivingWorldSocialEvent const& event)
{
    if (!IsValidSocialEvent(event))
        return false;

    LivingWorldSocialEdge updated;
    auto existing = FindRelation(event.actorId, event.otherId);
    if (existing.has_value())
    {
        if (event.simulationMinute < existing->lastInteractionSimulationMinute)
            return false;
        updated = *existing;
    }
    else
    {
        updated.actorId = event.actorId;
        updated.otherId = event.otherId;
    }

    auto clampSigned = [](std::int32_t value)
    {
        value = std::max<std::int32_t>(MinimumSignedBasisPoints, value);
        value = std::min<std::int32_t>(MaximumSignedBasisPoints, value);
        return static_cast<std::int16_t>(value);
    };
    auto clampUnsigned = [](std::int32_t value)
    {
        value = std::max<std::int32_t>(0, value);
        value = std::min<std::int32_t>(MaximumUnsignedBasisPoints, value);
        return static_cast<std::uint16_t>(value);
    };

    updated.affinityBasisPoints = clampSigned(
        static_cast<std::int32_t>(updated.affinityBasisPoints) + event.affinityDeltaBasisPoints);
    updated.trustBasisPoints = clampUnsigned(
        static_cast<std::int32_t>(updated.trustBasisPoints) + event.trustDeltaBasisPoints);
    updated.familiarityBasisPoints = clampUnsigned(
        static_cast<std::int32_t>(updated.familiarityBasisPoints) + event.familiarityDeltaBasisPoints);
    updated.rivalryBasisPoints = clampUnsigned(
        static_cast<std::int32_t>(updated.rivalryBasisPoints) + event.rivalryDeltaBasisPoints);
    updated.lastInteractionSimulationMinute = event.simulationMinute;

    return UpsertRelation(updated);
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
