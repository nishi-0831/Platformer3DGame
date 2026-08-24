#include "RectDetector.h"
#include <cmath>
#include "Core/Game.h"
#include "Core/ISystem.h"
#include "Components/Collider/ColliderCP.h"
#include "WindowContext/WindowContextUtil.h"
#include "WindowContext/CameraResource.h"
#include "Camera/CameraSystem.h"
#include "Core/Entity.h"
#include "Screen.h"
#include "Collision/Intersection.h"
using namespace mtgb;

mtgb::RectDetector::RectDetector(const RectDetectorConfig& _config)
	: config { _config }
{
}

mtgb::RectDetector::RectDetector(RectDetectorConfig&& _config)
	: config { std::move(_config) }
{
}

void mtgb::RectDetector::UpdateDetection()
{
	UpdateDetection(config);
}

void mtgb::RectDetector::UpdateDetection(RectDetectorConfig& _config)
{
	// 基底クラスの detectedTargets_を更新
	detectedTargets_.clear();

	Intersection::RectContains(
		_config.detectionRect,
		_config.base.targetTag,
		&detectedTargets_, // 基底クラスのメンバを使用
		_config.base.windowContext
	);

	CameraHandleInScene hCamera		 = WinCtxRes::Get<CameraResource>(_config.base.windowContext).GetHCamera();
	const Transform& cameraTransform = Game::System<CameraSystem>().GetTransform(hCamera);

	// 設定に合致しない要素を取り除く
	detectedTargets_.erase(
		std::remove_if(
			detectedTargets_.begin(),
			detectedTargets_.end(),
			[&](const ScreenCoordContainsInfo& _info)
			{
				Vector3 toTarget = _info.worldPos - cameraTransform.GetWorldPosition();
				Vector3 normal	 = cameraTransform.Forward();
				float distance	 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(toTarget, normal));

				// 設定した距離より遠いなら除く
				if (std::abs(distance) > _config.base.maxDistance)
				{
					return true;
				}

				return false;
			}
		),
		detectedTargets_.end()
	);
}

void mtgb::RectDetector::UpdateAndSetDetection(RectDetectorConfig& _config)
{
	config = _config;
	UpdateDetection();
}

void mtgb::RectDetector::UpdateAndSetDetection(RectDetectorConfig&& _config)
{
	config = std::move(_config);
	UpdateDetection();
}

RectF mtgb::RectDetector::GetDetectionArea() const
{
	Vector2F ratio = Game::System<Screen>().GetSizeRatio();
	float scale	   = (std::min)(ratio.x, ratio.y);

	float scaledSize  = config.detectionRect.size.x * scale;
	Vector2F center	  = Game::System<Screen>().GetSizeF() * 0.5f;
	Vector2F newPoint = center - Vector2F { scaledSize, scaledSize } * 0.5f;
	return { newPoint, { scaledSize, scaledSize } };
}

bool mtgb::RectDetector::IsLineOfSight(const Vector3& _cameraPos, const ScreenCoordContainsInfo& _targetInfo)
{
	Vector3 toTarget	   = Vector3::Normalize(_targetInfo.worldPos - _cameraPos);
	ColliderCP& colliderCP = Game::System<ColliderCP>();

	Intersection::RaycastInfo info;
	for (const auto& other : detectedTargets_)
	{
		// 自分自身とは判定をしない
		if (_targetInfo.entityId == other.entityId)
			continue;

		// 遮られていたら false
		if (colliderCP.RayCastHit(_targetInfo.worldPos, toTarget, FLT_MAX, &info, other.entityId))
		{
			return false;
		}
	}
	return true;
}
