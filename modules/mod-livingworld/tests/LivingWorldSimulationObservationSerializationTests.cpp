#include "LivingWorldSimulationObservationSerialization.h"

#include <cassert>
#include <iostream>
#include <string>

using namespace LivingWorld;

int main()
{
    SimulationObservationSummary summary;
    summary.retainedObservationCount = 5;
    summary.terminalObservationCount = 1;
    summary.clampedObservationCount = 1;
    summary.correlatedObservationCount = 4;
    summary.uniqueCorrelationCount = 2;
    summary.firstSimulationMinute = 100;
    summary.latestSimulationMinute = 250;
    summary.coveredSimulationMinutes = 150;
    summary.minimumProgressBasisPoints = 500;
    summary.maximumProgressBasisPoints = 10000;
    summary.historyClamped = true;

    auto encoded = SimulationObservationSerializer::SerializeSummary(summary);
    assert(encoded.has_value());
    assert(encoded->size() <= SimulationObservationSerializer::MaxSerializedBytes);

    auto decoded = SimulationObservationSerializer::DeserializeSummary(*encoded);
    assert(decoded.has_value());
    assert(decoded->retainedObservationCount == summary.retainedObservationCount);
    assert(decoded->terminalObservationCount == summary.terminalObservationCount);
    assert(decoded->clampedObservationCount == summary.clampedObservationCount);
    assert(decoded->correlatedObservationCount == summary.correlatedObservationCount);
    assert(decoded->uniqueCorrelationCount == summary.uniqueCorrelationCount);
    assert(decoded->firstSimulationMinute == summary.firstSimulationMinute);
    assert(decoded->latestSimulationMinute == summary.latestSimulationMinute);
    assert(decoded->coveredSimulationMinutes == summary.coveredSimulationMinutes);
    assert(decoded->minimumProgressBasisPoints == summary.minimumProgressBasisPoints);
    assert(decoded->maximumProgressBasisPoints == summary.maximumProgressBasisPoints);
    assert(decoded->historyClamped == summary.historyClamped);
    assert(SimulationObservationSerializer::SerializeSummary(*decoded) == encoded);

    assert(!SimulationObservationSerializer::DeserializeSummary("LWOS2|1").has_value());
    assert(!SimulationObservationSerializer::DeserializeSummary("LWOS1|1|2|0|0|0|0|0|0|0|0|0").has_value());
    assert(!SimulationObservationSerializer::DeserializeSummary("LWOS1|1|0|0|0|0|10|9|0|0|0|0").has_value());
    assert(!SimulationObservationSerializer::DeserializeSummary(std::string(1025, 'x')).has_value());

    SimulationCorrelationDiagnostic diagnostic;
    diagnostic.checkpointCorrelationId = "checkpoint-A_01.test";
    diagnostic.observationCount = 3;
    diagnostic.firstSimulationMinute = 100;
    diagnostic.latestSimulationMinute = 220;
    diagnostic.latestProgressBasisPoints = 10000;
    diagnostic.latestTerminal = true;
    diagnostic.anyClamped = false;

    auto encodedDiagnostic = SimulationObservationSerializer::SerializeDiagnostic(diagnostic);
    assert(encodedDiagnostic.has_value());
    auto decodedDiagnostic = SimulationObservationSerializer::DeserializeDiagnostic(*encodedDiagnostic);
    assert(decodedDiagnostic.has_value());
    assert(decodedDiagnostic->checkpointCorrelationId == diagnostic.checkpointCorrelationId);
    assert(decodedDiagnostic->observationCount == diagnostic.observationCount);
    assert(decodedDiagnostic->latestSimulationMinute == diagnostic.latestSimulationMinute);
    assert(decodedDiagnostic->latestTerminal);
    assert(SimulationObservationSerializer::SerializeDiagnostic(*decodedDiagnostic) == encodedDiagnostic);

    diagnostic.checkpointCorrelationId = "unsafe|identifier";
    assert(!SimulationObservationSerializer::SerializeDiagnostic(diagnostic).has_value());
    diagnostic.checkpointCorrelationId = "unsafe:identifier";
    assert(!SimulationObservationSerializer::SerializeDiagnostic(diagnostic).has_value());
    diagnostic.checkpointCorrelationId = std::string(
        SimulationRunObservationLimits::MaximumCheckpointCorrelationLength + 1, 'a');
    assert(!SimulationObservationSerializer::SerializeDiagnostic(diagnostic).has_value());
    assert(!SimulationObservationSerializer::DeserializeDiagnostic("LWOD1|unsafe id|1|0|0|0|0|0").has_value());

    std::cout << "LivingWorld simulation observation-serialization tests passed.\n";
    return 0;
}
