#include "Light.h"

namespace
{
	mtgb::Vector4 lightDir { 0, 1, 1, 0 };
}

Vector4 mtgb::Light::GetDir()
{
	return lightDir;
}
