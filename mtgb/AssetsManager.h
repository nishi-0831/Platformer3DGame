#pragma once
#include "Core/ISystem.h"

namespace mtgb
{
	class AssetsManager : public ISystem
	{
	  public:
		AssetsManager();
		~AssetsManager();

		void Initialize() override;
		void Update() override;
	};
} // namespace mtgb
