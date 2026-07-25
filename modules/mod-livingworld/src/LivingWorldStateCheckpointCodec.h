#ifndef MOD_LIVINGWORLD_STATE_CHECKPOINT_CODEC_H
#define MOD_LIVINGWORLD_STATE_CHECKPOINT_CODEC_H

#include "LivingWorldStateCheckpoint.h"

#include <optional>
#include <string>

namespace LivingWorld
{
    namespace StateCheckpointCodecLimits
    {
        inline constexpr std::size_t MaximumEncodedSize = 512;
    }

    class StateCheckpointCodec
    {
    public:
        static std::optional<std::string> Encode(StateCheckpoint const& checkpoint);
        static std::optional<StateCheckpoint> Decode(std::string const& encoded);
    };
}

#endif
