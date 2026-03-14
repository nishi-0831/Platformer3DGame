
#pragma once
#include <refl-cpp/refl.hpp>
#include "Vector3.h"

class Command;
struct Vector3Show : refl::attr::usage::type
{
	Command* operator()(mtgb::Vector3* _vec, const char* _name) const;
};

struct QuaternionShow : refl::attr::usage::type
{
	Command* operator()(DirectX::XMVECTORF32* _vec, const char* _name) const;
};

struct Vector4Show : refl::attr::usage::type
{
	Command* operator()(DirectX::XMVECTOR* _vec, const char* _name) const;
};

/// <summary>
/// 行列の変更をUndo/Redo機能に対応できていない。
/// </summary>
struct MatrixShow : refl::attr::usage::type
{
	Command* operator()(DirectX::XMMATRIX* _mat, const char* _name) const;
};
