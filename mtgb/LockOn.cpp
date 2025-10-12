#include "LockOn.h"
#include <algorithm>
#include "Draw.h"
//#include ""
void mtgb::LockOn::Search()
{
	rectDetector.UpdateDetection();

	// ���[���h���W�n�ň�ԋ߂��G��_��

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
		pTargetInfo = &(*it); // �A�h���X����
		reticleRect.x = pTargetInfo->screenPos.x - reticleSideExtents;
		reticleRect.y = pTargetInfo->screenPos.y - reticleSideExtents;
	}
	else
	{
		pTargetInfo = nullptr; // ������Ȃ������ꍇ�� nullptr
	}
}

void mtgb::LockOn::Shoot()
{
	if (rectDetector.HasDetectedTargets())
	{
		Vector3 toTarget = Vector3::Normalize(pTargetInfo->worldPos - pTransform->GetWorldPosition());
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
	//�����ő_�����߂�͈͂�`��
	const Vector2Int DRAW_SIZE{ lockOnSide ,lockOnSide };
	Draw::Image(frameImage, rectDetector.config.detectionRect, uiParams);

	//�_������܂��Ă���G�������\��
	if (rectDetector.HasDetectedTargets())
	{
		//Draw::Image(lockOnReticle_, lockOn_->reticleRect, lockOn_->uiParams);
	}
}
