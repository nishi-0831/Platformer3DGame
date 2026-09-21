#pragma once
#include "Saw.h"

namespace mtgb
{
	class MovingSaw : public Saw
	{
	  public:
		MovingSaw();
		~MovingSaw();

		void Update() override;

	  private:
		Interpolator* pInterpolator_;
		static unsigned int generateCounter_;
	};
} // namespace mtgb