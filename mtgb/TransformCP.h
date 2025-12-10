#pragma once
#include "ISystem.h"
#include "ComponentPool.h"


namespace mtgb
{
	class Transform;

	/// <summary>
	/// Transformコンポーネントのプール
	/// </summary>
	class TransformCP :
		public ComponentPool<Transform,TransformCP>
	{
	public:
		TransformCP();
		//TransformCP(const TransformCP&) = default;
		~TransformCP();

		void Update() override;
	};
}
