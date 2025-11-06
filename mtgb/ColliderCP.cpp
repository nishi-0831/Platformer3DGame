#include "ColliderCP.h"
#include "RigidBody.h"
#include "Matrix4x4.h"
#include "Transform.h"
#include "Fbx.h"
#include "Debug.h"

#include "GameObject.h"
#include "RectInt.h"
#include "SceneSystem.h"
#include "CameraSystem.h"
#include "Screen.h"
mtgb::ColliderCP::ColliderCP()
{
}

mtgb::ColliderCP::~ColliderCP()
{
}

void mtgb::ColliderCP::Start()
{
	Collider::hSphereModel_ = Fbx::Load("Model/SphereCollider.fbx");
	massert(Collider::hSphereModel_ >= 0
		&& "SphereColliderÉÇÉfÉãÇÃì«Ç›çûÇ›Ç…é∏îs @ColliderCP::Start");

	Collider::hBoxModel_ = Fbx::Load("Model/BoxCollider.fbx");
	massert(Collider::hBoxModel_ >= 0
		&& "BoxColliderÉÇÉfÉãÇÃì«Ç›çûÇ›Ç…é∏îs @ColliderCP::Start");
}

void mtgb::ColliderCP::Update()
{
	static Matrix4x4 matrix{};
	for (size_t i = 0; i < poolId_.size(); i++)
	{
		if (poolId_[i] != INVALID_ENTITY)
		{
			pool_[i].onColliders_.clear();

			pool_[i].UpdateBoundingData();
		}
	}

	for (size_t i = 0; i < poolId_.size(); i++)
	{
		if (poolId_[i] != INVALID_ENTITY && i < poolId_.size() - 1)
		{
			for (size_t j = i + 1; j < poolId_.size(); j++)
			{
				if (poolId_[j] != INVALID_ENTITY)
				{
					if (pool_[i].IsHit(pool_[j]))
					{
						Collider& a = pool_[i];
						Collider& b = pool_[j];
						a.onColliders_.insert(&b);
						b.onColliders_.insert(&a);

						DirectX::BoundingBox* aabb{ nullptr };
						DirectX::BoundingSphere* sphere{ nullptr };

						// âüÇµèoÇµåüèÿÇÃÇΩÇﬂÇ…ãÖÇ™PlayeràÍëÃÇæÇØÇ∆Ç¢Ç§ëOíÒ

						if (a.colliderType == ColliderType::TYPE_SPHERE)
						{
							/*
								Vector3 movement = RigidBody::ColliderSphere(a.computeSphere_,b.computeBox_); 
								if(a.isKinematic == false)
								{
									RigidBody& rigidBody = RigidBody::Get(poolId_[i]);
									rigidBody.velocity_ += movement;
								}
							*/
							Vector3 push = RigidBody::GetPushAmount(a.computeSphere_, b.computeBox_);
							Transform& transform = Transform::Get(poolId_[i]);
							transform.position += push;
						}
						else if (b.colliderType == ColliderType::TYPE_SPHERE)
						{
							/*
								Vector3 movement = RigidBody::ColliderSphere(b.computeSphere_,a.computeBox_); 
								if(b.isKinematic == false)
								{
									RigidBody& rigidBody = RigidBody::Get(poolId_[j]);
									rigidBody.velocity_ += movement;
								}
							*/
							Vector3 push = RigidBody::GetPushAmount(b.computeSphere_, a.computeBox_);
							Transform& transform = Transform::Get(poolId_[j]);
							transform.position += push;
						}
					}
				}
			}
		}
	}
}

void mtgb::ColliderCP::TestDraw() const
{

	//Game::System<MTImGui>().Begin("ColliderCP");
	LOGF("ColliderCP BEGIN\n");
	for (size_t i = 0; i < poolId_.size(); i++)
	{
		if (poolId_[i] != INVALID_ENTITY)
		{
			LOGF("EID:%d colSize=%d\n ", poolId_[i], pool_[i].onColliders_.size());
			LOGIMGUI("EID:%d colSize=%d ", poolId_[i], pool_[i].onColliders_.size());
		}
	}
	LOGF("ColliderCP END\n");
}
void mtgb::ColliderCP::Draw()
{
	for (size_t i = 0; i < poolId_.size(); i++)
	{
		if (poolId_[i] != INVALID_ENTITY)
		{
			pool_[i].Draw();
		}
	}
}
mtgb::EntityId mtgb::ColliderCP::RayCastHitAll(const Vector3& _origin, const Vector3& _dir, float dist)
{
	EntityId nearestEntity = INVALID_ENTITY;

	for (size_t i = 0; i < poolId_.size(); i++)
	{
		if (poolId_[i] != INVALID_ENTITY)
		{

			float nearest = dist;
			Transform* pTransform{ nullptr };
			float distance = 0.0f;
			//pool_[i]
			//for (auto& collider : colliders)
			{
				//if (collider->IsHit(_origin, _dir, &distance))
				EntityId id = poolId_[i];
				if(RayCastHit(_origin,_dir,&distance,id))
				{
					//Game::System<TransformCP>().TryGet(pTransform, id);
					if(distance < nearest)
					{
						nearest = distance;
						nearestEntity = id;
					}
				}
			}
		}
	}
	return nearestEntity;
}

bool mtgb::ColliderCP::RayCastHit(const Vector3& _origin, const Vector3& _dir, float* dist, EntityId _entityId)
{
	std::vector<Collider*> colliders{};
	if (!TryGet(&colliders, _entityId))
		return false;

	for (auto& collider : colliders)
	{
		return collider->IsHit(_origin, _dir, dist);
	}
	return false;
}

void mtgb::ColliderCP::IsHitAll(const Vector3& _center, float _radius, std::vector<EntityId>* _entityIds)
{
	_entityIds->clear();

	for (const auto& collider : pool_)
	{
		if (collider.IsHit(_center, _radius))
		{
			_entityIds->push_back(collider.GetEntityId());
		}
	}
}

void mtgb::ColliderCP::RectContains(const RectF& _rect, const std::string& _name, std::vector<ScreenCoordContainsInfo>* _info, WindowContext _context)
{
	_info->clear();

	std::vector<GameObject*> foundGameObjects;
	Game::System<SceneSystem>().GetActiveScene()->GetGameObjects(_name, &foundGameObjects);
	if (foundGameObjects.empty()) return;
	RectContainsImpl(_rect, foundGameObjects, _info, _context);
}

void mtgb::ColliderCP::RectContains(const RectF& _rect, GameObjectTag _tag, std::vector<ScreenCoordContainsInfo>* _info, WindowContext _context)
{
	_info->clear();

	std::vector<GameObject*> foundGameObjects;
	Game::System<SceneSystem>().GetActiveScene()->GetGameObjects(_tag, &foundGameObjects);
	if (foundGameObjects.empty()) return;
	RectContainsImpl(_rect, foundGameObjects, _info, _context);
}

void mtgb::ColliderCP::RectContainsImpl(const RectF& _rect,const std::vector<GameObject*>& _objs, std::vector<ScreenCoordContainsInfo>* _info, WindowContext _context)
{
	CameraSystem& camSys = Game::System<CameraSystem>();
	const WorldToScreenData& data = camSys.GetWorldToScreenData(_context);
	Vector2F ratio = Game::System<Screen>().GetSizeRatio();
	for (auto& object : _objs)
	{
		Vector3 worldPos = object->Component<Transform>()->GetWorldPosition();
		Vector3 screenPos = camSys.GetWorldToScreenPos(worldPos, data);
		/*if (screenPos.z < 0)
			continue;*/
			/*if (RectF::Contains(screenPos, RectF(_rect.x ,_rect.y , _rect.width , _rect.height )))
			{
				_info->emplace_back(worldPos,screenPos,object->GetEntityId());
			}*/
		if (RectF::Contains(Vector2F{ screenPos.x,screenPos.y }, RectF(_rect.x * ratio.x, _rect.y * ratio.y, _rect.width * ratio.x, _rect.height * ratio.y)))
		{
			_info->emplace_back(worldPos, screenPos, object->GetEntityId());
		}
	}
}
