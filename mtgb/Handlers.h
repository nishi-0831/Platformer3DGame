#pragma once
#include <cstdint>


#pragma region Handlers

namespace mtgb
{
	/// <summary>
	/// �摜�̃n���h��
	/// </summary>
	using ImageHandle = int32_t;
	/// <summary>
	/// �����̃n���h��
	/// </summary>
	using AudioHandle = int32_t;
	/// <summary>
	/// ���f���̃n���h��
	/// </summary>
	using ModelHandle = int32_t;
	/// <summary>
	/// OBJ���f���̃n���h��
	/// </summary>
	using OBJModelHandle = int32_t;
	/// <summary>
	/// FBX���f���̃n���h��
	/// </summary>
	using FBXModelHandle = int32_t;

	/// <summary>
	/// �e�L�X�g�̃n���h��
	/// </summary>
	using TextHandle = int32_t;

	using CameraHandleInScene = int32_t;

	/// <summary>
	/// �V�[�����Œʗp����J�����n���h��
	/// </summary>
	using CameraHandleInScene = int32_t;

	/// <summary>
	/// �����ȃn���h���l
	/// </summary>
	static const int32_t INVALID_HANDLE{ -1 };
}

#pragma endregion
