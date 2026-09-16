#pragma once
#include <mtgb.h>
class RespawnManager : public mtgb::GameObject, public mtgb::ImGuiShowable
{
  public:
	RespawnManager();
	~RespawnManager();
	void Update() override;
	void ShowImGui() override;

  private:
	// 現在のリスポーンポイントに割り当てられている優先度
	int currentRespawnPointPriority_;
	EntityId playerEntityId_;
	EntityId currentRespawnPointEntityId_;
};

/// <summary>
/// プレイヤーが落下した際の復活場所
/// </summary>
class RespawnPoint : public mtgb::GameObject, public mtgb::ImGuiShowable
{
  public:
	RespawnPoint();
	~RespawnPoint();
	void Update() override;
	void ShowImGui() override;

  private:
	void OnCollisionEnter(EntityId _entityId);
	/// <summary>
	/// 複数の復活場所がある場合の優先度
	/// </summary>
	int priority_;
	Transform* pTransform_;
	Collider* pCollider_;
	RigidBody* pRigidBody_;

	static unsigned int generateCounter_;
};