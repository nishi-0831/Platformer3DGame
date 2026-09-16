
#pragma once
#include "cmtgb.h"
#include "Core/ISystem.h"
#include "Core/Entity.h"
namespace mtgb
{
	/// <summary>
	/// EntityIdのカウントを管理する
	/// </summary>
	class EntityManager : public ISystem
	{
	  public:
		void Initialize() override;
		void Update() override;
		/// <summary>
		/// エンティティ作成
		/// </summary>
		/// <returns></returns>
		EntityId CreateEntity();
		/// <summary>
		/// EntityIdのカウントを返す
		/// </summary>
		/// <returns></returns>
		EntityId GetCounter();
		/// <summary>
		/// カウントを設定する
		/// </summary>
		/// <param name="_counter"></param>
		void ChangeCounter(EntityId _counter);
		/// <summary>
		/// カウントをデクリメントする
		/// </summary>
		void DecrementCounter();

	  private:
		EntityId entityCounter_;
	};
} // namespace mtgb