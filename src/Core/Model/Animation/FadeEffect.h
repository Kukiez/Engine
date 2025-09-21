#pragma once
#include "Interpolation.h"

struct FadeEffect {
    double duration;
    EasingFunction easing;

    explicit FadeEffect(const double duration = 0, const EasingFunction function = Easing::linear) : duration(duration), easing(function) {}
};

using FadeIn = FadeEffect;
using FadeOut = FadeEffect;