#ifndef MOD_LIVINGWORLD_MEMORY_SOCIAL_GRAPH_H
#define MOD_LIVINGWORLD_MEMORY_SOCIAL_GRAPH_H

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace LivingWorld
{
enum class MemoryKind : std::uint8_t
{
    Encounter = 0,
    Cooperation = 1,
    Conflict = 2,
    Trade = 3,
    Organization = 4
};

struct LivingWorldMemoryRecord
{
    std::uint64_t actorId = 0;
    std::uint64_t subjectId = 0;
    std::uint64_t simulationMinute = 0;
    std::uint16_t salienceBasisPoints = 0;
    std::int16_t valenceBasisPoints = 0;
    std::uint16_t decayBasisPointsPerDay = 0;
    bool protectedMemory = false;
    MemoryKind kind = MemoryKind::Encounter;
    std::string tag;
};

struct LivingWorldSocialEdge
{
    std::uint64_t actorId = 0;
    std::uint64_t otherId = 0;
    std::int16_t affinityBasisPoints = 0;
    std::uint16_t trustBasisPoints = 0;
    std::uint16_t familiarityBasisPoints = 0;
    std::uint16_t rivalryBasisPoints = 0;
    std::uint64_t lastInteractionSimulationMinute = 0;
};

struct LivingWorldSocialEvent
{
    std::uint64_t actorId = 0;
    std::uint64_t otherId = 0;
    std::uint64_t simulationMinute = 0;
    std::int16_t affinityDeltaBasisPoints = 0;
    std::int16_t trustDeltaBasisPoints = 0;
    std::int16_t familiarityDeltaBasisPoints = 0;
    std::int16_t rivalryDeltaBasisPoints = 0;
};

class LivingWorldMemorySocialGraph
{
public:
    static constexpr std::size_t MaximumMemories = 256;
    static constexpr std::size_t MaximumProtectedMemories = 32;
    static constexpr std::size_t MaximumRelations = 128;
    static constexpr std::size_t MaximumTagBytes = 48;
    static constexpr std::uint16_t MinimumMeaningfulSalienceBasisPoints = 100;
    static constexpr std::uint16_t MaximumUnsignedBasisPoints = 10000;
    static constexpr std::uint16_t MaximumDecayBasisPointsPerDay = 1000;
    static constexpr std::int16_t MinimumSignedBasisPoints = -10000;
    static constexpr std::int16_t MaximumSignedBasisPoints = 10000;
    static constexpr std::int16_t MaximumSocialEventDeltaBasisPoints = 1000;
    static constexpr std::uint64_t SimulationMinutesPerDay = 1440;

    bool AddMemory(LivingWorldMemoryRecord record);
    bool UpsertRelation(LivingWorldSocialEdge edge);
    bool ApplySocialEvent(LivingWorldSocialEvent const& event);

    std::vector<LivingWorldMemoryRecord> const& Memories() const;
    std::vector<LivingWorldSocialEdge> const& Relations() const;
    std::optional<LivingWorldSocialEdge> FindRelation(std::uint64_t actorId, std::uint64_t otherId) const;

    static std::optional<std::uint16_t> EffectiveSalienceBasisPoints(
        LivingWorldMemoryRecord const& record,
        std::uint64_t atSimulationMinute);
    static bool IsMeaningfulAt(LivingWorldMemoryRecord const& record, std::uint64_t atSimulationMinute);

    void Reset();

private:
    static bool IsSafeTag(std::string const& tag);
    static bool IsValidMemoryKind(MemoryKind kind);
    static bool IsValidMemory(LivingWorldMemoryRecord const& record);
    static bool IsValidRelation(LivingWorldSocialEdge const& edge);
    static bool IsValidSocialEvent(LivingWorldSocialEvent const& event);

    std::vector<LivingWorldMemoryRecord> memories_;
    std::vector<LivingWorldSocialEdge> relations_;
};
}

#endif
