#pragma once
#include "Core/Entity.h"
class IActor
{
  public:
	IActor(EntityId _entityId);
	virtual ~IActor();
	/// <summary>
	/// 他のアクターから踏まれた際の処理
	/// </summary>
	/// <param name="_pOther">自分を踏んだアクター</param>
	virtual void OnStomped(IActor* _pOther) = 0;
	/// <summary>
	/// 他のアクターが横から衝突した際の処理
	/// </summary>
	/// <param name="_pOther">自分に横から衝突したアクター</param>
	virtual void OnHitSide(IActor* _pOther) = 0;
	/// <summary>
	/// ダメージを受ける際の処理
	/// </summary>
	/// <param name="_damage">受けるダメージ</param>
	virtual void TakeDamage(int _damage) = 0;
	/// <summary>
	/// 外力を与える
	/// </summary>
	/// <param name="_velocity"></param>
	virtual void AddExternalVelocity(const Vector3& _velocity) {};
	EntityId GetId()
	{
		return id_;
	}

  private:
	IActor() = delete;
	EntityId id_;
};