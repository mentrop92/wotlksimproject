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

bool LivingWorldMemorySocialGraph::IsValidMemory(LivingWorldMemoryRecord const& record)
{
    if (record.actorId == 0 || record.subjectId == 0)
        return false;
    if (record.salienceBasisPoints > MaximumUnsignedBasisPoints)
        return false;
    return IsSafeTag(record.tag);
}

bool LivingWorldMemorySocialGraph::IsValidRelation(LivingWorldSocialEdge const& edge)
{
    if (edge.actorId == 0 || edge.otherId == 0 || edge.actorId == edge.otherId)
        return false;
    if (edge.affinityBasisPoints < MinimumAffinityBasisPoints || edge.affinityBasisPoints > MaximumAffinityBasisPoints)
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

    memories_.push_back(std::move(record));
    if (memories_.size() > MaximumMemories)
        memories_.erase(memories_.begin());
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

void LivingWorldMemorySocialGraph::Reset()
{
    memories_.clear();
    relations_.clear();
}
}
