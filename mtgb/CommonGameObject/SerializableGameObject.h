#pragma once
#include <mtgb.h>

namespace mtgb
{
	class SerializableGameObject : public GameObject
	{
	  public:
		SerializableGameObject();
		~SerializableGameObject();

	  private:
		static unsigned int generateCounter_;
	};
} // namespace mtgb