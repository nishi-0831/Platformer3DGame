#include "Intersection.h"
#include <cfloat>
#include <algorithm>
#include <DirectXMath.h>
#include "Math/Quaternion.h"
#include "Math/Matrix4x4.h"
#include "Core/GameObject/GameObject.h"
#include "Camera/CameraSystem.h"
#include "Screen.h"
#include "Core/SceneSystem.h"

namespace mtgb
{
	namespace Intersection
	{
		DirectX::XMVECTORF32 unitVectorEpsilon { FLT_EPSILON, FLT_EPSILON, FLT_EPSILON, FLT_EPSILON };
		bool XMVECTORIsUnit(DirectX::FXMVECTOR _v)
		{
			DirectX::XMVECTOR difference =
				DirectX::XMVectorSubtract(DirectX::XMVector3Length(_v), DirectX::XMVectorSplatOne());
			return DirectX::XMVector4Less(DirectX::XMVectorAbs(difference), unitVectorEpsilon);
		}

		void RectContainsImpl(
			const RectF& _rect,
			const std::vector<GameObject*>& _objs,
			std::vector<ScreenCoordContainsInfo>* _info,
			WindowContext _context
		)
		{
			CameraSystem& camSys		  = Game::System<CameraSystem>();
			const WorldToScreenData& data = camSys.GetWorldToScreenData(_context);
			Vector2F ratio				  = Game::System<Screen>().GetSizeRatio();
			for (auto& object : _objs)
			{
				Vector3 worldPos  = object->Component<Transform>()->GetWorldPosition();
				Vector3 screenPos = camSys.GetWorldToScreenPos(worldPos, data);
				if (RectF::Contains(
						Vector2F { screenPos.x, screenPos.y },
						RectF(_rect.x * ratio.x, _rect.y * ratio.y, _rect.width * ratio.x, _rect.height * ratio.y)
					))
				{
					_info->emplace_back(worldPos, screenPos, object->GetEntityId());
				}
			}
		}
		bool IsHit(
			const DirectX::BoundingSphere& _sphere,
			const Vector3& _origin,
			const Vector3& _dir,
			float _maxDistance,
			RaycastInfo* _info
		)
		{
			bool result = false;
			float dist	= _maxDistance;
			/////
			// DirectXCollision.hのBoundingSphere::Intersectsをコピペした。
			// 何故かIntersectsに実引数が正常に渡されないから。
			/////
			using namespace DirectX;
			// まずBoundingSphereを最新状態に更新

			// 方向ベクトルを正規化（元のベクトルは保持）
			Vector3 normalizedDir = Vector3::Normalize(_dir);

			XMVECTOR vNormalizeDir = XMLoadFloat3(&normalizedDir);
			if (!XMVECTORIsUnit(vNormalizeDir))
			{
				return false;
			}

			XMVECTOR vCenter = XMLoadFloat3(&_sphere.Center);
			XMVECTOR vRadius = XMVectorReplicatePtr(&_sphere.Radius);

			// 球の中心からレイの原点へのベクトル
			XMVECTOR l = XMVectorSubtract(vCenter, _origin);

			// lをレイの方向に射影したスカラー
			// 球の中心とレイの最も近い点の距離
			XMVECTOR s = XMVector3Dot(l, normalizedDir);

			// 球の中心からレイの原点への距離の二乗
			XMVECTOR l2 = XMVector3Dot(l, l);

			// 半径の二乗
			XMVECTOR r2 = XMVectorMultiply(vRadius, vRadius);

			// 球の中心からレイへの垂線の二乗
			// 三平方の定理でいうとlが斜辺にあたる

			XMVECTOR m2 = XMVectorNegativeMultiplySubtract(s, s, l2);

			XMVECTOR NoIntersection;

			// もしレイの原点が球の外側、かつ
			// 球の中心がレイの原点の後ろにあるならば接触していない
			NoIntersection = XMVectorAndInt(
				// 射影ベクトルが負ならレイの方向と反対、レイの原点より後ろにある
				XMVectorLess(s, XMVectorZero())
				// l2の方が大きいなら球の外側にある
				,
				XMVectorGreater(l2, r2)
			);

			// 球の中心からレイへの垂線が球の半径よりも大きいか
			// レイの最も近い地点が球の外側にあるか

			if (XMVector4Greater(m2, r2))
			{
				return false;
			}

			// 衝突しているとして、最も近い地点を計算

			// 球の中心とレイの最も近い点から球の表面までの距離
			XMVECTOR q = XMVectorSqrt(XMVectorSubtract(r2, m2));

			// r2-m2が負の値の場合はqに0を入れる
			XMVECTOR mask = XMVectorGreater(m2, r2);
			XMVECTOR zero = XMVectorZero();
			// maskがtrueの場合はzero、falseの場合はq
			q = XMVectorSelect(q, zero, mask);

			// レイの原点から球との交点までの距離
			// 手前
			XMVECTOR t1 = XMVectorSubtract(s, q);
			// 奥
			XMVECTOR t2 = XMVectorAdd(s, q);

			// レイの原点が球の内側か
			XMVECTOR originInside = XMVectorLessOrEqual(l2, r2);

			// 第三引数がtrueなら第二引数、falseなら第一引数
			XMVECTOR t = XMVectorSelect(t1, t2, originInside);

			if (XMVector4NotEqualInt(NoIntersection, XMVectorTrueInt()))
			{
				DirectX::XMStoreFloat(&dist, t);
				if (dist <= _maxDistance)
				{
					result			= true;
					_info->point	= _origin + dist * _dir;
					_info->normal	= Vector3::Normalize(_info->point - _sphere.Center);
					_info->distance = dist;
				}
			}

			return result;
		}

		bool IsHit(
			const DirectX::BoundingBox& _aabb,
			const Vector3& _origin,
			const Vector3& _dir,
			float _maxDistance,
			RaycastInfo* _info
		)
		{
			float dist	= _maxDistance;
			bool result = _aabb.Intersects(_origin, _dir, dist);
			if (result == false)
			{
				return false;
			}
			if (dist > _maxDistance)
			{
				return false;
			}

			_info->distance	 = dist;
			Vector3 hitPoint = _origin + _dir * dist;
			_info->point	 = hitPoint;
			Vector3 extents	 = _aabb.Extents;
			Vector3 center	 = _aabb.Center;
			Vector3 p		 = hitPoint - center;

			float dx = std::abs(extents.x - std::abs(p.x));
			float dy = std::abs(extents.y - std::abs(p.y));
			float dz = std::abs(extents.z - std::abs(p.z));
			// xが最も近い
			if (dx < dy && dx < dz)
			{
				_info->normal = Vector3(std::copysign(1.0f, p.x), 0.0f, 0.0f);
			}
			// yが最も近い
			else if (dy < dz)
			{
				_info->normal = Vector3(0.0f, std::copysign(1.0f, p.y), 0.0f);
			}
			// zが最も近い
			else
			{
				_info->normal = Vector3(0.0f, 0.0f, std::copysign(1.0f, p.z));
			}
			return result;
		}

		bool IsHit(
			const DirectX::BoundingOrientedBox& _obb,
			const Vector3& _origin,
			const Vector3& _dir,
			float _maxDistance,
			RaycastInfo* _info
		)
		{
			float dist	= _maxDistance;
			bool result = _obb.Intersects(_origin, _dir, dist);
			if (result == false)
			{
				return false;
			}
			if (dist > _maxDistance)
			{
				return false;
			}
			_info->distance		   = dist;
			Vector3 hitPoint	   = _origin + _dir * dist;
			_info->point		   = hitPoint;
			Vector3 extents		   = _obb.Extents;
			Vector3 center		   = _obb.Center;
			Vector3 p			   = hitPoint - center;
			Quaternion rotate	   = _obb.Orientation;
			Matrix4x4 rotMat	   = DirectX::XMMatrixRotationQuaternion(rotate);
			Vector3 rotatedExtents = DirectX::XMVector3TransformCoord(extents, rotMat);
			rotatedExtents.x	   = std::abs(rotatedExtents.x);
			rotatedExtents.y	   = std::abs(rotatedExtents.y);
			rotatedExtents.z	   = std::abs(rotatedExtents.z);
			float dx			   = std::abs(rotatedExtents.x - std::abs(p.x));
			float dy			   = std::abs(rotatedExtents.y - std::abs(p.y));
			float dz			   = std::abs(rotatedExtents.z - std::abs(p.z));

			// xが最も近い
			if (dx < dy && dx < dz)
			{
				_info->normal = Vector3(std::copysign(1.0f, p.x), 0.0f, 0.0f);
			}
			// yが最も近い
			else if (dy < dz)
			{
				_info->normal = Vector3(0.0f, std::copysign(1.0f, p.y), 0.0f);
			}
			else
			{
				_info->normal = Vector3(0.0f, 0.0f, std::copysign(1.0f, p.z));
			}
			_info->normal = _info->normal * rotMat;

			return result;
		}

		std::optional<IntersectInfo> Intersect(
			const DirectX::BoundingSphere& _sphere,
			const DirectX::BoundingBox& _aabb
		)
		{
			IntersectInfo info;

			Vector3 aabbMin = _aabb.Center - _aabb.Extents;
			Vector3 aabbMax = _aabb.Center + _aabb.Extents;

			// 最短地点
			Vector3 closest;

			// 各座標軸にクランプする
			closest.x = std::clamp(_sphere.Center.x, aabbMin.x, aabbMax.x);
			closest.y = std::clamp(_sphere.Center.y, aabbMin.y, aabbMax.y);
			closest.z = std::clamp(_sphere.Center.z, aabbMin.z, aabbMax.z);

			Vector3 v  = _sphere.Center - closest;
			float dist = v.Size();

			// 距離がほぼゼロの場合
			if (dist <= FLT_EPSILON)
			{
				// AABBの中心から球の中心への方向
				v = Vector3::Normalize(_sphere.Center - _aabb.Center);
				// 方向が決まらなければ押し出しはしない
				if (v.Size() <= FLT_EPSILON)
					return std::nullopt;
				dist = 0.0f;
			}
			else
			{
				v = Vector3::Normalize(v);
			}

			// 押し出し量
			float penetration = _sphere.Radius - dist;
			if (penetration <= 0.0f)
				return std::nullopt;

			info.closest = closest;
			// 最短地点から球の中心へ押し出す
			info.push = v * penetration;
			return info;
		}

		std::optional<IntersectInfo> Intersect(
			const DirectX::BoundingSphere& _sphere,
			const DirectX::BoundingOrientedBox& _obb
		)
		{
			IntersectInfo info;

			Vector3 localCenter	   = _sphere.Center - _obb.Center;
			Vector3 extents		   = _obb.Extents;
			Quaternion rotate	   = _obb.Orientation;
			Matrix4x4 rotMat	   = DirectX::XMMatrixRotationQuaternion(rotate);
			Vector3 rotatedExtents = DirectX::XMVector3TransformCoord(extents, rotMat);
			rotatedExtents.x	   = std::abs(rotatedExtents.x);
			rotatedExtents.y	   = std::abs(rotatedExtents.y);
			rotatedExtents.z	   = std::abs(rotatedExtents.z);

			Vector3 closest;
			closest.x = std::clamp(localCenter.x, -rotatedExtents.x, rotatedExtents.x);
			closest.y = std::clamp(localCenter.y, -rotatedExtents.y, rotatedExtents.y);
			closest.z = std::clamp(localCenter.z, -rotatedExtents.z, rotatedExtents.z);

			Vector3 v	   = localCenter - closest;
			float distance = v.Size();

			if (distance <= FLT_EPSILON)
			{
				v = Vector3::Normalize(_sphere.Center - _obb.Center);
				// 方向が決まらなければ押し出しはしない
				if (v.Size() <= FLT_EPSILON)
					return std::nullopt;
				distance = 0.0f;
			}

			float penetration = _sphere.Radius - distance;
			if (penetration <= 0.0f)
				return std::nullopt;

			info.closest = closest;
			info.push	 = Vector3::Normalize(v) * penetration;
			return info;
		}

		void mtgb::Intersection::RectContains(
			const RectF& _rect,
			std::string_view _name,
			std::vector<ScreenCoordContainsInfo>* _info,
			WindowContext _context
		)
		{
			_info->clear();

			std::vector<GameObject*> foundGameObjects;
			Game::System<SceneSystem>().GetActiveScene()->GetGameObjects(_name, &foundGameObjects);
			if (foundGameObjects.empty())
				return;
			RectContainsImpl(_rect, foundGameObjects, _info, _context);
		}

		void mtgb::Intersection::RectContains(
			const RectF& _rect,
			GameObjectTag _tag,
			std::vector<ScreenCoordContainsInfo>* _info,
			WindowContext _context
		)
		{
			_info->clear();

			std::vector<GameObject*> foundGameObjects;
			Game::System<SceneSystem>().GetActiveScene()->GetGameObjects(_tag, &foundGameObjects);
			if (foundGameObjects.empty())
				return;
			RectContainsImpl(_rect, foundGameObjects, _info, _context);
		}
	} // namespace Intersection
} // namespace mtgb