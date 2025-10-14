#pragma once
#include <mtgb.h>

namespace mtgb
{
	class Camera : public GameObject
	{
	public:
		Camera();
		~Camera();

		void Update() override;
		void Draw() const override;
	private:
		Transform* pTransform_;
	};
}

