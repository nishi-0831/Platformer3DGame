#pragma once
#include <mtgb.h>
#include <vector>

namespace mtgb
{
	/// <summary>
	/// プレイヤーのHPを表示するゲームオブジェクト
	/// </summary>
	class HPViewer : public GameObject
	{
	  public:
		HPViewer(int _hpCount);
		~HPViewer();

		void Update() override;
		void Draw() const override;
		void Start() override;

		void TakeDamage(int _damage);

	  private:
		std::vector<ImageRenderer*> pHpImages_;
		Vector2F pos_;
		Vector2F size_;
	};
} // namespace mtgb