#include "LivingWorldRuntimeState.h"

namespace LivingWorld
{
    RuntimeState& RuntimeState::Instance()
    {
        static RuntimeState instance;
        return instance;
    }

    RuntimeState::RuntimeState() : _window(RuntimeMetricLimits::MaximumWindowSamples) { }

    void RuntimeState::Reset()
    {
        _window.Reset();
        _latest = RuntimeObservationSnapshot{};
        _hasSamples = false;
    }

    RuntimeObservationSnapshot RuntimeState::Record(RuntimeObservationInput const& input)
    {
        _latest = RuntimeObservability::Build(input);
        _window.Add(_latest);
        _hasSamples = true;
        return _latest;
    }

    RuntimeObservationSnapshot const& RuntimeState::Latest() const
    {
        return _latest;
    }

    RuntimeMetricSummary RuntimeState::Summary() const
    {
        return _window.Summarize();
    }

    bool RuntimeState::HasSamples() const
    {
        return _hasSamples;
    }
}
