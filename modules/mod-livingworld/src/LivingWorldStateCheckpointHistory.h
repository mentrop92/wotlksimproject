#ifndef MOD_LIVINGWORLD_STATE_CHECKPOINT_HISTORY_H
#define MOD_LIVINGWORLD_STATE_CHECKPOINT_HISTORY_H

#include "LivingWorldStateCheckpoint.h"

#include <cstddef>
#include <optional>
#include <vector>

namespace LivingWorld
{
    namespace StateCheckpointHistoryLimits
    {
        inline constexpr std::size_t MaximumEntries = 64;
    }

    class StateCheckpointHistory
    {
    public:
        explicit StateCheckpointHistory(std::size_t capacity = StateCheckpointHistoryLimits::MaximumEntries);

        bool Push(StateCheckpoint const& checkpoint);
        std::optional<StateCheckpoint> Latest() const;
        std::optional<StateCheckpoint> FindAtOrBefore(std::uint64_t simulationMinute) const;
        std::size_t Size() const;
        std::size_t Capacity() const;
        void Clear();

    private:
        std::size_t _capacity;
        std::vector<StateCheckpoint> _entries;
    };
}

#endif
