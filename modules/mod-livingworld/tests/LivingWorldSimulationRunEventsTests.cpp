#include "LivingWorldSimulationRunEvents.h"

#include <cassert>
#include <iostream>
#include <string>

using namespace LivingWorld;

int main()
{
    SimulationRunRequest request;
    request.startMinute = 100;
    request.durationMinutes = 120;
    request.tickMinutes = 60;
    request.multiplier = 10;

    SimulationRunPlan const plan = SimulationRunPlanner::Build(request);
    assert(plan.valid);

    SimulationRunState state = SimulationRunState::Create(plan);
    SimulationRunEventHistory history;

    assert(history.AppendSnapshot(state.GetSnapshot(), "created"));
    assert(state.Start());
    assert(history.AppendSnapshot(state.GetSnapshot(), "started"));
    assert(state.Advance(60));
    assert(history.AppendSnapshot(state.GetSnapshot(), "halfway"));
    assert(state.Advance(60));
    assert(history.AppendSnapshot(state.GetSnapshot(), "completed"));

    assert(history.Size() == 4);
    assert(history.Events()[0].type == SimulationRunEventType::Created);
    assert(history.Events()[1].type == SimulationRunEventType::Started);
    assert(history.Events()[2].type == SimulationRunEventType::Progress);
    assert(history.Events()[3].type == SimulationRunEventType::Completed);
    assert(history.Events()[3].progressBasisPoints == 10000);

    SimulationRunEvent backwards;
    backwards.simulationMinute = 1;
    assert(!history.Append(backwards));
    assert(history.Size() == 4);

    SimulationRunEvent unsafe;
    unsafe.simulationMinute = 221;
    unsafe.progressBasisPoints = 50000;
    unsafe.detail = std::string(120, 'x');
    unsafe.detail[0] = '\n';
    assert(history.Append(unsafe));
    SimulationRunEvent const& sanitized = history.Events().back();
    assert(sanitized.progressBasisPoints == 10000);
    assert(sanitized.detail.size() == SimulationRunEventLimits::MaximumDetailLength);
    assert(sanitized.detail[0] == ' ');
    assert(sanitized.clamped);

    history.Reset();
    assert(history.Empty());

    for (std::size_t index = 0; index < SimulationRunEventLimits::MaximumEvents + 12; ++index)
    {
        SimulationRunEvent event;
        event.simulationMinute = index;
        event.type = SimulationRunEventType::Progress;
        assert(history.Append(event));
    }

    assert(history.Size() == SimulationRunEventLimits::MaximumEvents);
    assert(history.Events().front().simulationMinute == 12);
    assert(history.Events().back().simulationMinute == SimulationRunEventLimits::MaximumEvents + 11);

    std::cout << "LivingWorld simulation run-event tests passed.\n";
    return 0;
}
