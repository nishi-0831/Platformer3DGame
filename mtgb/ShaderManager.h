#pragma once
#include <array>
#include "Core/ISystem.h"
#include "ShaderType.h"
#include "Shader/IShader.h"
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
		/// <summary>
		/// ShaderTypeの列挙値に対応するシェーダーを取得する
		/// </summary>
		/// <param name="_shaderType"></param>
		/// <returns></returns>
		IShader& GetShader(ShaderType _shaderType);

	  private:
		std::array<IShader*, static_cast<int8_t>(ShaderType::MAX)> shaders_;
	};
} // namespace mtgb