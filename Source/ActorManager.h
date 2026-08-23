#pragma once
#include <list>
#include "IActor.h"
#include "ISystem.h"

class ActorManager : public ISystem
{
  public:
	// ISystem を介して継承されました
	void Initialize() override;
	void Update() override;
	/// <summary>
	/// アクターを登録
	/// ActorManagerはポインタの解放を行わないので、呼び出し側で寿命の管理をしてください
	/// </summary>
	/// <param name="_pActor"></param>
	void RegisterActor(IActor* _pActor);
	/// <summary>
	/// EntityIdに対応するアクターを登録解除
	/// </summary>
	/// <param name="_id"></param>
	void UnregisterActor(EntityId _id);
	/// <summary>
	/// アクターのポインタを返す
	/// </summary>
	/// <param name="_id"></param>
	/// <returns>引数のIdに合致するアクターのポインタ。いない場合はnullptr</returns>
	IActor* GetActor(EntityId _id);

  private:
	std::list<IActor*> actors_;
};