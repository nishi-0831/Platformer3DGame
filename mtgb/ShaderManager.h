#pragma once
#include <array>
#include "ISystem.h"
#include "ShaderType.h"
#include "IShader.h"
namespace mtgb
{
	class ShaderManager : public ISystem
	{
	  public:
		void Initialize() override;
		void Update() override;
		/// <summary>
		/// 解放処理
		/// </summary>
		void Release() override;
		std::array<IShader*, static_cast<int8_t>(ShaderType::MAX)> shader_;
		IShader& GetShader(ShaderType _shaderType);
	};
} // namespace mtgb