#include "LockOn.h"
#include <algorithm>
#include "Draw.h"
// #include ""
void mtgb::LockOn::Search()
{
	rectDetector.UpdateDetection();

	// ワールド座標系で一番近い敵を狙う

	auto it = std::min_element(
		rectDetector.detectedTargets.begin(),
		rectDetector.detectedTargets.end(),
		[this](const RectContainsInfo& a, const RectContainsInfo& b)
		{
			float da = (pTransform->position - a.worldPos).Size();
			float db = (pTransform->position - b.worldPos).Size();
			return da < db;
		}
	);

	if (it != rectDetector.detectedTargets.end())
	{
		pTargetInfo	  = &(*it); // アドレスを代入
		reticleRect.x = pTargetInfo->screenPos.x - reticleSideExtents;
		reticleRect.y = pTargetInfo->screenPos.y - reticleSideExtents;
	}
	else
	{
		pTargetInfo = nullptr; // 見つからなかった場合は nullptr
	}
}

void mtgb::LockOn::Shoot()
{
	if (rectDetector.HasDetectedTargets())
	{
		Vector3 toTarget	= Vector3::Normalize(pTargetInfo->worldPos - pTransform->GetWorldPosition());
		Quaternion shootDir = Quaternion::LookRotation(toTarget, Vector3::Up());
		Instantiate<PlayerBullet>(pTransform->GetWorldPosition(), shootDir);
	}
}

mtgb::Vector3 mtgb::LockOn::GetTargetPos()
{
	return pTargetInfo->worldPos;
}

void mtgb::LockOn::Draw() const
{
	// 自動で狙いを定める範囲を描画
	const Vector2Int DRAW_SIZE { lockOnSide, lockOnSide };
	Draw::Image(frameImage, rectDetector.config.detectionRect, uiParams);

	// 狙いが定まっている敵を強調表示
	if (rectDetector.HasDetectedTargets())
	{
		// Draw::Image(lockOnReticle_, lockOn_->reticleRect, lockOn_->uiParams);
	}
}
