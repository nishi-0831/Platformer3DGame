#include "stdafx.h"
#include "BoxPlane.h"
#include <DirectXMath.h>
#include <vector>
#include <tuple>
#include <algorithm>
#include <cmath>
//XMGLOBALCONST XMVECTORF32 g_BoxOffset[8] =
//{
//    { { { -1.0f, -1.0f,  1.0f, 0.0f } } },
//    { { {  1.0f, -1.0f,  1.0f, 0.0f } } },
//    { { {  1.0f,  1.0f,  1.0f, 0.0f } } },
//    { { { -1.0f,  1.0f,  1.0f, 0.0f } } },
//    { { { -1.0f, -1.0f, -1.0f, 0.0f } } },
//    { { {  1.0f, -1.0f, -1.0f, 0.0f } } },
//    { { {  1.0f,  1.0f, -1.0f, 0.0f } } },
//    { { { -1.0f,  1.0f, -1.0f, 0.0f } } },
//};

Plane GetPlane(const DirectX::BoundingBox& _aabb, BoxPlaneDir _planeDir)
{
    // 平面の法線
    Vector3 normal;
    // 平面内の3の点
    Vector3 point1;
    Vector3 point2;
    Vector3 point3;

    // AABBの各頂点
    Vector3 corners[8];
    // DirectXCollision.iniにあるg_BoxOffsetの順番で頂点を取得
    _aabb.GetCorners(corners);

    switch (_planeDir)
    {
    case BoxPlaneDir::FRONT:
        normal = Vector3::Forward();
        point1 = corners[0];
        point2 = corners[2];
        point3 = corners[1];
        break;
    case BoxPlaneDir::BACK:
        // 後面のインデックス : 4,5,6,7
        normal = Vector3::Back();
        point1 = corners[4];
        point2 = corners[5];
        point3 = corners[6];
        break;
    case BoxPlaneDir::RIGHT:
        // 右側面のインデックス : 1,2,6,5
        normal = Vector3::Right();
        point1 = corners[6];
        point2 = corners[5];
        point3 = corners[1];
        break;
    case BoxPlaneDir::LEFT:
        // 左側面のインデックス : 0,3,7,4
        normal = Vector3::Left();
        point1 = corners[0];
        point2 = corners[7];
        point3 = corners[3];
        break;
    case BoxPlaneDir::TOP:
        // 上面のインデックス : 3,2,6,7
        normal = Vector3::Up();
        point1 = corners[3];
        point2 = corners[6];
        point3 = corners[2];
        break;
    case BoxPlaneDir::BOTTOM:
        // 下面のインデックス : 0,1,5,4
        normal = Vector3::Down();
        point1 = corners[0];
        point2 = corners[1];
        point3 = corners[5];
        break;
    }

    // 三つの点から平面の方程式を計算
    Vector3 planeEquation = DirectX::XMPlaneFromPoints(point1, point2, point3);
    return Plane{ .equation = planeEquation, .normal = normal };
}

float GetPlaneAngleRad(const Plane& _plane, const Vector3& _vec)
{
    float dot = DirectX::XMPlaneDotNormal(_plane.equation, Vector3::Normalize(_vec)).m128_f32[0];
    dot = std::clamp(dot, -1.0f, 1.0f);
    return std::acosf(dot);
}

float GetPlaneAngleRad(const DirectX::BoundingBox& _aabb, BoxPlaneDir _planeDir, const Vector3& _vec)
{
    Plane plane = GetPlane(_aabb, _planeDir);
    return GetPlaneAngleRad(plane, _vec);
}

std::optional<Plane> GetSmallestAnglePlane(const DirectX::BoundingBox& _aabb, const Vector3& _vec)
{
    Vector3 vecNormal = Vector3::Normalize(_vec);
    if (vecNormal.x == 0.0f && vecNormal.y == 0.0f && vecNormal.z == 0.0f)
        return std::nullopt;

    float bestDot = -1.0f;
    std::optional<Plane> bestPlane;
    for (int i = 0; i < 6; i++)
    {
        Plane p = GetPlane(_aabb, static_cast<BoxPlaneDir>(i));
        float dot = DirectX::XMPlaneDotNormal(p.equation, vecNormal).m128_f32[0];
        
        if (dot > bestDot)
        {
            bestDot = dot;
            bestPlane = p;
        }
    }
    return bestPlane;
}
