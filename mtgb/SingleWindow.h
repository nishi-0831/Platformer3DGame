#pragma once
#include "ISystem.h"
#include "WindowContext.h"
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


		void SetDoubleWindowPos();
	private:
		WindowContext context_;
	};
}