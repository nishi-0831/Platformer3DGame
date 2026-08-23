#pragma once
#include "Core/ISystem.h"
#include "Core/Component/ComponentPool.h"

namespace mtgb
{
	class Transform;

	/// <summary>
	/// Transformコンポーネントのプール
	/// </summary>
	class TransformCP : public ComponentPool<Transform, TransformCP>
	{
	  public:
		TransformCP();
		~TransformCP();

		void Update() override;
	};
} // namespace mtgb
