#ifndef MOD_LIVINGWORLD_RUNTIME_STATE_H
#define MOD_LIVINGWORLD_RUNTIME_STATE_H

#include "LivingWorldRuntimeMetrics.h"

namespace LivingWorld
{
    class RuntimeState
    {
    public:
        static RuntimeState& Instance();

        void Reset();
        RuntimeObservationSnapshot Record(RuntimeObservationInput const& input);
        RuntimeObservationSnapshot const& Latest() const;
        RuntimeMetricSummary Summary() const;
        bool HasSamples() const;

    private:
        RuntimeState();

        RuntimeMetricWindow _window;
        RuntimeObservationSnapshot _latest;
        bool _hasSamples = false;
    };
}

#define sLivingWorldRuntimeState LivingWorld::RuntimeState::Instance()

#endif
