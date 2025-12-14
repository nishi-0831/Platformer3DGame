#pragma once
#include "ISystem.h"
#include "ComponentPool.h"

namespace mtgb
{
	class Interpolator;

	class InterpolatorCP :
		public ComponentPool<Interpolator, InterpolatorCP>
	{
	public:
		InterpolatorCP();
		~InterpolatorCP();

		void Update() override;
	};
}