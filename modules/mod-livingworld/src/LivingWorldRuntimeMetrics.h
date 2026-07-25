#ifndef MOD_LIVINGWORLD_RUNTIME_METRICS_H
#define MOD_LIVINGWORLD_RUNTIME_METRICS_H

#include "LivingWorldRuntimeObservability.h"

#include <cstddef>
#include <cstdint>
#include <deque>

namespace LivingWorld
{
    namespace RuntimeMetricLimits
    {
        inline constexpr std::size_t MaximumWindowSamples = 60;
    }

    struct RuntimeMetricSummary
    {
        std::uint32_t sampleCount = 0;
        std::uint32_t unhealthySamples = 0;
        std::uint32_t emergencyDrainSamples = 0;
        std::uint32_t constrainedTargetSamples = 0;
        std::uint32_t candidateShortfallSamples = 0;
        std::uint64_t requestedOperations = 0;
        std::uint64_t selectedCandidates = 0;
        std::uint64_t unmetOperations = 0;
        std::uint64_t blockedCandidates = 0;
        std::uint32_t averageUpdateTimeMs = 0;
        std::uint32_t averageDatabaseLatencyMs = 0;
        std::uint32_t peakUpdateTimeMs = 0;
        std::uint32_t peakDatabaseLatencyMs = 0;
    };

    class RuntimeMetricWindow
    {
    public:
        explicit RuntimeMetricWindow(std::size_t maximumSamples = 12);

        void Add(RuntimeObservationSnapshot const& snapshot);
        void Reset();
        RuntimeMetricSummary Summarize() const;
        std::size_t Size() const;
        std::size_t Capacity() const;

    private:
        std::size_t _maximumSamples;
        std::deque<RuntimeObservationSnapshot> _samples;
    };
}

#endif
