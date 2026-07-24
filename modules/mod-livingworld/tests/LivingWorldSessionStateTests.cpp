#include "LivingWorldSessionState.h"

#include <cassert>
#include <iostream>

using namespace LivingWorld;

int main()
{
    SessionState state;
    state.motivation = 70;
    state.fatigue = 10;
    state.frustration = 10;
    state.burnout = 5;

    SessionSignals productive;
    productive.elapsedMinutes = 60;
    productive.plannedMinutes = 180;
    productive.activeGameplay = true;
    productive.socialActivity = true;
    productive.meaningfulSuccess = true;

    SessionState improved = SessionStateModel::Update(state, productive, 50, 60);
    assert(improved.motivation > state.motivation);
    assert(improved.frustration < state.frustration);
    assert(improved.logoutReadiness < 50);

    SessionSignals overrun;
    overrun.elapsedMinutes = 240;
    overrun.plannedMinutes = 180;
    overrun.activeGameplay = true;
    overrun.meaningfulFailure = true;

    SessionState tired = improved;
    for (int i = 0; i < 12; ++i)
        tired = SessionStateModel::Update(tired, overrun, 90, 20);

    assert(tired.fatigue >= 75);
    assert(tired.frustration > improved.frustration);
    assert(tired.burnout > improved.burnout);
    assert(tired.logoutReadiness >= 70);

    SessionSignals combat = overrun;
    combat.inCombat = true;
    SessionState combatState = SessionStateModel::Update(tired, combat, 90, 20);
    assert(combatState.logoutReadiness < tired.logoutReadiness);

    std::cout << "LivingWorld session state tests passed.\n";
    return 0;
}
