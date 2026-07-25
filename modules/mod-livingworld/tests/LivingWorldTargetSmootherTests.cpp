#include "LivingWorldTargetSmoother.h"

#include <cassert>
#include <iostream>

using namespace LivingWorld;

int main()
{
    TargetSmoother smoother;
    TargetSmoothingPolicy policy;
    policy.maximumRisePerMinute = 30;
    policy.maximumFallPerMinute = 60;
    policy.recoveryHoldMs = 120000;

    smoother.Reset(100);

    TargetSmoothingResult result = smoother.Update(60000, 200, false, false, policy);
    assert(result.smoothedTarget == 130);
    assert(result.riseLimited);

    result = smoother.Update(60000, 200, false, false, policy);
    assert(result.smoothedTarget == 160);

    result = smoother.Update(60000, 40, true, false, policy);
    assert(result.smoothedTarget == 100);
    assert(result.fallLimited);

    // Load pressure has cleared, but recovery remains held to avoid an immediate
    // login surge. Falling targets are still allowed during the hold.
    result = smoother.Update(60000, 200, false, false, policy);
    assert(result.smoothedTarget == 100);
    assert(result.recoveryHeld);

    result = smoother.Update(60000, 200, false, false, policy);
    assert(result.smoothedTarget == 130);
    assert(result.riseLimited);

    // Emergency drains bypass downward smoothing, but never upward smoothing.
    result = smoother.Update(1000, 10, true, true, policy);
    assert(result.smoothedTarget == 10);
    assert(!result.fallLimited);

    result = smoother.Update(60000, 500, false, false, policy);
    assert(result.smoothedTarget == 10);
    assert(result.recoveryHeld);

    std::cout << "LivingWorld target smoother tests passed.\n";
    return 0;
}
