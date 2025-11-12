#pragma once

namespace mtgb
{
	struct CameraData
	{
		float distance;
		float orbitSpeed;
		float minPolarAngleRad;
		float maxPolarAngleRad;
		float minAzimuthalAngleRad_;
		float maxAzimuthalAngleRad_;
	};
}