#include "LivingWorldStateCheckpointHistory.h"

#include <algorithm>

namespace LivingWorld
{
    StateCheckpointHistory::StateCheckpointHistory(std::size_t capacity)
        : _capacity(std::clamp<std::size_t>(capacity, 1, StateCheckpointHistoryLimits::MaximumEntries))
    {
        _entries.reserve(_capacity);
    }

    bool StateCheckpointHistory::Push(StateCheckpoint const& checkpoint)
    {
        if (!StateCheckpointBuilder::IsCompatible(checkpoint))
            return false;

        if (!_entries.empty() && checkpoint.simulationMinute < _entries.back().simulationMinute)
            return false;

        if (_entries.size() == _capacity)
            _entries.erase(_entries.begin());

        _entries.push_back(checkpoint);
        return true;
    }

    std::optional<StateCheckpoint> StateCheckpointHistory::Latest() const
    {
        if (_entries.empty())
            return std::nullopt;
        return _entries.back();
    }

    std::optional<StateCheckpoint> StateCheckpointHistory::FindAtOrBefore(std::uint64_t simulationMinute) const
    {
        auto const itr = std::upper_bound(
            _entries.begin(),
            _entries.end(),
            simulationMinute,
            [](std::uint64_t minute, StateCheckpoint const& checkpoint)
            {
                return minute < checkpoint.simulationMinute;
            });

        if (itr == _entries.begin())
            return std::nullopt;
        return *std::prev(itr);
    }

    std::size_t StateCheckpointHistory::Size() const
    {
        return _entries.size();
    }

    std::size_t StateCheckpointHistory::Capacity() const
    {
        return _capacity;
    }

    void StateCheckpointHistory::Clear()
    {
        _entries.clear();
    }
}
