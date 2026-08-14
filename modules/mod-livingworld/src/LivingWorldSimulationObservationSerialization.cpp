#include "LivingWorldSimulationObservationSerialization.h"

#include <array>
#include <charconv>
#include <cctype>
#include <string_view>
#include <vector>

namespace LivingWorld
{
namespace
{
std::vector<std::string_view> Split(std::string const& payload)
{
    std::vector<std::string_view> fields;
    std::string_view view(payload);
    std::size_t start = 0;
    while (start <= view.size())
    {
        std::size_t end = view.find('|', start);
        fields.emplace_back(view.substr(start, end == std::string_view::npos ? view.size() - start : end - start));
        if (end == std::string_view::npos)
            break;
        start = end + 1;
    }
    return fields;
}

template <typename T>
bool ParseUnsigned(std::string_view text, T& value)
{
    if (text.empty())
        return false;
    T parsed{};
    auto result = std::from_chars(text.data(), text.data() + text.size(), parsed);
    if (result.ec != std::errc() || result.ptr != text.data() + text.size())
        return false;
    value = parsed;
    return true;
}

bool ParseBool(std::string_view text, bool& value)
{
    if (text == "0")
    {
        value = false;
        return true;
    }
    if (text == "1")
    {
        value = true;
        return true;
    }
    return false;
}
}

bool SimulationObservationSerializer::IsSafeCorrelationId(std::string const& value)
{
    if (value.empty() || value.size() > MaxCorrelationIdBytes)
        return false;

    for (unsigned char c : value)
    {
        if (!(std::isalnum(c) || c == '-' || c == '_' || c == '.'))
            return false;
    }
    return true;
}

std::optional<std::string> SimulationObservationSerializer::SerializeSummary(SimulationObservationSummary const& value)
{
    std::string payload = "LWOS1|" + std::to_string(value.retainedObservationCount) + "|" +
        std::to_string(value.terminalObservationCount) + "|" +
        std::to_string(value.clampedObservationCount) + "|" +
        std::to_string(value.correlatedObservationCount) + "|" +
        std::to_string(value.uniqueCorrelationCount) + "|" +
        std::to_string(value.firstSimulationMinute) + "|" +
        std::to_string(value.latestSimulationMinute) + "|" +
        std::to_string(value.coveredSimulationMinutes) + "|" +
        std::to_string(value.minimumProgressBasisPoints) + "|" +
        std::to_string(value.maximumProgressBasisPoints) + "|" +
        (value.historyClamped ? "1" : "0");

    if (payload.size() > MaxSerializedBytes)
        return std::nullopt;
    return payload;
}

std::optional<SimulationObservationSummary> SimulationObservationSerializer::DeserializeSummary(std::string const& payload)
{
    if (payload.size() > MaxSerializedBytes)
        return std::nullopt;

    auto fields = Split(payload);
    if (fields.size() != 12 || fields[0] != "LWOS1")
        return std::nullopt;

    SimulationObservationSummary value;
    if (!ParseUnsigned(fields[1], value.retainedObservationCount) ||
        !ParseUnsigned(fields[2], value.terminalObservationCount) ||
        !ParseUnsigned(fields[3], value.clampedObservationCount) ||
        !ParseUnsigned(fields[4], value.correlatedObservationCount) ||
        !ParseUnsigned(fields[5], value.uniqueCorrelationCount) ||
        !ParseUnsigned(fields[6], value.firstSimulationMinute) ||
        !ParseUnsigned(fields[7], value.latestSimulationMinute) ||
        !ParseUnsigned(fields[8], value.coveredSimulationMinutes) ||
        !ParseUnsigned(fields[9], value.minimumProgressBasisPoints) ||
        !ParseUnsigned(fields[10], value.maximumProgressBasisPoints) ||
        !ParseBool(fields[11], value.historyClamped))
        return std::nullopt;

    if (value.terminalObservationCount > value.retainedObservationCount ||
        value.clampedObservationCount > value.retainedObservationCount ||
        value.correlatedObservationCount > value.retainedObservationCount ||
        value.uniqueCorrelationCount > value.correlatedObservationCount ||
        value.minimumProgressBasisPoints > 10000 || value.maximumProgressBasisPoints > 10000 ||
        value.minimumProgressBasisPoints > value.maximumProgressBasisPoints ||
        value.latestSimulationMinute < value.firstSimulationMinute ||
        value.coveredSimulationMinutes != value.latestSimulationMinute - value.firstSimulationMinute)
        return std::nullopt;

    return value;
}

std::optional<std::string> SimulationObservationSerializer::SerializeDiagnostic(SimulationCorrelationDiagnostic const& value)
{
    if (!IsSafeCorrelationId(value.checkpointCorrelationId))
        return std::nullopt;

    std::string payload = "LWOD1|" + value.checkpointCorrelationId + "|" +
        std::to_string(value.observationCount) + "|" +
        std::to_string(value.firstSimulationMinute) + "|" +
        std::to_string(value.latestSimulationMinute) + "|" +
        std::to_string(value.latestProgressBasisPoints) + "|" +
        (value.latestTerminal ? "1" : "0") + "|" +
        (value.anyClamped ? "1" : "0");

    if (payload.size() > MaxSerializedBytes)
        return std::nullopt;
    return payload;
}

std::optional<SimulationCorrelationDiagnostic> SimulationObservationSerializer::DeserializeDiagnostic(std::string const& payload)
{
    if (payload.size() > MaxSerializedBytes)
        return std::nullopt;

    auto fields = Split(payload);
    if (fields.size() != 8 || fields[0] != "LWOD1")
        return std::nullopt;

    SimulationCorrelationDiagnostic value;
    value.checkpointCorrelationId = std::string(fields[1]);
    if (!IsSafeCorrelationId(value.checkpointCorrelationId) ||
        !ParseUnsigned(fields[2], value.observationCount) ||
        !ParseUnsigned(fields[3], value.firstSimulationMinute) ||
        !ParseUnsigned(fields[4], value.latestSimulationMinute) ||
        !ParseUnsigned(fields[5], value.latestProgressBasisPoints) ||
        !ParseBool(fields[6], value.latestTerminal) ||
        !ParseBool(fields[7], value.anyClamped))
        return std::nullopt;

    if (value.observationCount == 0 || value.latestSimulationMinute < value.firstSimulationMinute ||
        value.latestProgressBasisPoints > 10000)
        return std::nullopt;

    return value;
}
}
