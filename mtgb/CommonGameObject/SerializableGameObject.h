#pragma once
#include <mtgb.h>

namespace mtgb
{
	class SerializableGameObject : public GameObject
	{
	  public:
		SerializableGameObject();
		~SerializableGameObject();

		void Update() override;
		void Draw() const override;
		void Start() override;

	  private:
		static unsigned int generateCounter_;
	};
} // namespace mtgb