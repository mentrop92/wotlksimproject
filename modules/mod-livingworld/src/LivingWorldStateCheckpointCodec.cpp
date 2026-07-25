#include "LivingWorldStateCheckpointCodec.h"

#include <array>
#include <charconv>
#include <cctype>
#include <sstream>
#include <string_view>

namespace LivingWorld
{
    namespace
    {
        char HexDigit(std::uint8_t value)
        {
            return value < 10 ? static_cast<char>('0' + value) : static_cast<char>('A' + (value - 10));
        }

        std::optional<std::uint8_t> HexValue(char value)
        {
            if (value >= '0' && value <= '9')
                return static_cast<std::uint8_t>(value - '0');
            if (value >= 'A' && value <= 'F')
                return static_cast<std::uint8_t>(10 + value - 'A');
            if (value >= 'a' && value <= 'f')
                return static_cast<std::uint8_t>(10 + value - 'a');
            return std::nullopt;
        }

        std::string EncodeLabel(std::string const& label)
        {
            std::string encoded;
            encoded.reserve(label.size() * 2);
            for (unsigned char value : label)
            {
                encoded.push_back(HexDigit(static_cast<std::uint8_t>(value >> 4)));
                encoded.push_back(HexDigit(static_cast<std::uint8_t>(value & 0x0F)));
            }
            return encoded;
        }

        std::optional<std::string> DecodeLabel(std::string_view encoded)
        {
            if (encoded.size() % 2 != 0 || encoded.size() > StateCheckpointLimits::MaximumLabelLength * 2)
                return std::nullopt;

            std::string label;
            label.reserve(encoded.size() / 2);
            for (std::size_t index = 0; index < encoded.size(); index += 2)
            {
                std::optional<std::uint8_t> high = HexValue(encoded[index]);
                std::optional<std::uint8_t> low = HexValue(encoded[index + 1]);
                if (!high || !low)
                    return std::nullopt;
                label.push_back(static_cast<char>((*high << 4) | *low));
            }
            return label;
        }

        template <typename T>
        bool ParseUnsigned(std::string_view text, T& output)
        {
            if (text.empty())
                return false;
            T value{};
            auto const result = std::from_chars(text.data(), text.data() + text.size(), value);
            if (result.ec != std::errc{} || result.ptr != text.data() + text.size())
                return false;
            output = value;
            return true;
        }
    }

    std::optional<std::string> StateCheckpointCodec::Encode(StateCheckpoint const& checkpoint)
    {
        if (!StateCheckpointBuilder::IsCompatible(checkpoint))
            return std::nullopt;

        std::ostringstream output;
        output << "LWCP1|"
               << checkpoint.schemaVersion << '|'
               << checkpoint.simulationMinute << '|'
               << checkpoint.onlineAI << '|'
               << checkpoint.effectiveTarget << '|'
               << checkpoint.queuedOperations << '|'
               << (checkpoint.emergencyDrain ? 1 : 0) << '|'
               << (checkpoint.inputClamped ? 1 : 0) << '|'
               << EncodeLabel(checkpoint.label);

        std::string encoded = output.str();
        if (encoded.size() > StateCheckpointCodecLimits::MaximumEncodedSize)
            return std::nullopt;
        return encoded;
    }

    std::optional<StateCheckpoint> StateCheckpointCodec::Decode(std::string const& encoded)
    {
        if (encoded.empty() || encoded.size() > StateCheckpointCodecLimits::MaximumEncodedSize)
            return std::nullopt;

        std::array<std::string_view, 9> fields{};
        std::size_t fieldIndex = 0;
        std::size_t start = 0;
        while (fieldIndex < fields.size())
        {
            std::size_t const separator = encoded.find('|', start);
            if (separator == std::string::npos)
            {
                fields[fieldIndex++] = std::string_view(encoded).substr(start);
                break;
            }
            fields[fieldIndex++] = std::string_view(encoded).substr(start, separator - start);
            start = separator + 1;
        }

        if (fieldIndex != fields.size() || encoded.find('|', start) != std::string::npos || fields[0] != "LWCP1")
            return std::nullopt;

        StateCheckpoint checkpoint;
        std::uint32_t emergency = 0;
        std::uint32_t clamped = 0;
        if (!ParseUnsigned(fields[1], checkpoint.schemaVersion) ||
            !ParseUnsigned(fields[2], checkpoint.simulationMinute) ||
            !ParseUnsigned(fields[3], checkpoint.onlineAI) ||
            !ParseUnsigned(fields[4], checkpoint.effectiveTarget) ||
            !ParseUnsigned(fields[5], checkpoint.queuedOperations) ||
            !ParseUnsigned(fields[6], emergency) ||
            !ParseUnsigned(fields[7], clamped) ||
            emergency > 1 || clamped > 1)
        {
            return std::nullopt;
        }

        std::optional<std::string> label = DecodeLabel(fields[8]);
        if (!label)
            return std::nullopt;

        checkpoint.emergencyDrain = emergency != 0;
        checkpoint.inputClamped = clamped != 0;
        checkpoint.label = std::move(*label);
        if (!StateCheckpointBuilder::IsCompatible(checkpoint))
            return std::nullopt;
        return checkpoint;
    }
}
