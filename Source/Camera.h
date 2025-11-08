#pragma once
#include <mtgb.h>

namespace mtgb
{
	class Camera : public GameObject, public ICamera
	{
	public:
		Camera();
		Camera(GameObject* _pGameObj);
		~Camera();

		void Update() override;
		void Draw() const override;
	};
}

