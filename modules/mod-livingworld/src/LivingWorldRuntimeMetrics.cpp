#include "LivingWorldRuntimeMetrics.h"

#include <algorithm>
#include <limits>

namespace LivingWorld
{
    namespace
    {
        std::uint64_t SaturatingAdd(std::uint64_t left, std::uint64_t right)
        {
            if (right > std::numeric_limits<std::uint64_t>::max() - left)
                return std::numeric_limits<std::uint64_t>::max();
            return left + right;
        }
    }

    RuntimeMetricWindow::RuntimeMetricWindow(std::size_t maximumSamples)
        : _maximumSamples(std::clamp<std::size_t>(maximumSamples, 1, RuntimeMetricLimits::MaximumWindowSamples))
    {
    }

    void RuntimeMetricWindow::Add(RuntimeObservationSnapshot const& snapshot)
    {
        _samples.push_back(snapshot);
        while (_samples.size() > _maximumSamples)
            _samples.pop_front();
    }

    void RuntimeMetricWindow::Reset()
    {
        _samples.clear();
    }

    RuntimeMetricSummary RuntimeMetricWindow::Summarize() const
    {
        RuntimeMetricSummary summary;
        summary.sampleCount = static_cast<std::uint32_t>(_samples.size());
        if (_samples.empty())
            return summary;

        std::uint64_t totalUpdateTime = 0;
        std::uint64_t totalDatabaseLatency = 0;

        for (RuntimeObservationSnapshot const& snapshot : _samples)
        {
            summary.unhealthySamples += snapshot.healthy ? 0U : 1U;
            summary.emergencyDrainSamples += snapshot.bounded.emergencyDrain ? 1U : 0U;
            summary.constrainedTargetSamples += snapshot.targetConstrained ? 1U : 0U;
            summary.candidateShortfallSamples += snapshot.candidateShortfall ? 1U : 0U;
            summary.requestedOperations = SaturatingAdd(summary.requestedOperations, snapshot.bounded.requestedOperations);
            summary.selectedCandidates = SaturatingAdd(summary.selectedCandidates, snapshot.bounded.selectedCandidates);
            summary.unmetOperations = SaturatingAdd(summary.unmetOperations, snapshot.unmetOperations);
            summary.blockedCandidates = SaturatingAdd(summary.blockedCandidates, snapshot.totalBlockedCandidates);
            totalUpdateTime = SaturatingAdd(totalUpdateTime, snapshot.bounded.averageUpdateTimeMs);
            totalDatabaseLatency = SaturatingAdd(totalDatabaseLatency, snapshot.bounded.databaseLatencyMs);
            summary.peakUpdateTimeMs = std::max(summary.peakUpdateTimeMs, snapshot.bounded.averageUpdateTimeMs);
            summary.peakDatabaseLatencyMs = std::max(summary.peakDatabaseLatencyMs, snapshot.bounded.databaseLatencyMs);
        }

        summary.averageUpdateTimeMs = static_cast<std::uint32_t>(totalUpdateTime / summary.sampleCount);
        summary.averageDatabaseLatencyMs = static_cast<std::uint32_t>(totalDatabaseLatency / summary.sampleCount);
        return summary;
    }

    std::size_t RuntimeMetricWindow::Size() const
    {
        return _samples.size();
    }

    std::size_t RuntimeMetricWindow::Capacity() const
    {
        return _maximumSamples;
    }
}
