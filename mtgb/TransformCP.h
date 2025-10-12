#pragma once
#include "ISystem.h"
#include "ComponentPool.h"
#include "Transform.h"


namespace mtgb
{
	class Transform;

	/// <summary>
	/// Transform�R���|�[�l���g�̃v�[��
	/// </summary>
	class TransformCP :
		public ComponentPool<Transform>
	{
	public:
		TransformCP();
		TransformCP(const TransformCP&) = default;
		~TransformCP();

		void Update() override;
	};
}
