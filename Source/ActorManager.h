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
	/// ActorManagerはポインタの解放は行わないので、呼び出し側で寿命の管理をしてください
	/// </summary>
	/// <param name="_pActor"></param>
	void RegisterActor(IActor* _pActor);
	void UnregisterActor(EntityId _id);
	IActor* GetActor(EntityId _id);

  private:
	std::list<IActor*> actors_;
};