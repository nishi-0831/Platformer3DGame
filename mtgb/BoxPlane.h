#pragma once
#include "Vector3.h"
#include <DirectXCollision.h>
#include <optional>
enum class BoxPlaneDir
{
	FRONT,
	BACK,
	RIGHT,
	LEFT,
	TOP,
	BOTTOM
};

struct Plane
{
	Vector3 equation;
	Vector3 normal;
};


/// <summary>
/// AABBから指定された向きの平面を返す
/// </summary>
/// <param name="_aabb"> AABB </param>
/// <param name="_plane"> 平面の向き </param>
/// <returns> 平面方程式、法線を持つ平面の構造体 </returns>
Plane GetPlane(const DirectX::BoundingBox& _aabb, BoxPlaneDir _planeDir);


float GetPlaneAngleRad(const Plane& _plane, const Vector3& _vec);
float GetPlaneAngleRad(const DirectX::BoundingBox& _aabb, BoxPlaneDir _planeDir, const Vector3& _vec);

std::optional<Plane> GetSmallestAnglePlane(const DirectX::BoundingBox& _aabb, const Vector3& _vec);

