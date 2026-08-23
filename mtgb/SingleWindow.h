#pragma once
#include "Core/ISystem.h"
#include "WindowContext/WindowContext.h"
typedef struct HWND__* HWND;

namespace mtgb
{
	class SingleWindow : public ISystem
	{
	  public:
		SingleWindow();
		~SingleWindow();

		void Initialize() override;
		void Update() override;

	  private:
		WindowContext context_;
	};
} // namespace mtgb