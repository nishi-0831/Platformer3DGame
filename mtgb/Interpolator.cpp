#include "stdafx.h"
#include "Interpolator.h"
#include "GameTime.h"
#include "Mathf.h"
mtgb::Interpolator::Interpolator()
    : dir{1.0f}
    , elapsed{0.0f}
    , duration{1.0f}
    , start{Vector3::Zero()}
    , end{Vector3::One()}
{
}

void mtgb::Interpolator::UpdateProgress()
{
    elapsed += Time::DeltaTimeF() * dir;
    float progress = elapsed / duration;
    if (progress > 1.0f || progress < 0.0f)
    {
        dir *= -1.0f;
        elapsed = std::clamp(elapsed, 0.0f, 1.0f);
    }
}

mtgb::Vector3 mtgb::Interpolator::Evaluate()
{
    float progres = elapsed / duration;
    return Mathf::Lerp(start, end, progres);
}

void mtgb::Interpolator::SetEndpoints(const Vector3& _start, const Vector3& _end)
{
    start = _start;
    end = _end;
}
