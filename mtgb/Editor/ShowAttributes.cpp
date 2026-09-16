#include "ShowAttributes.h"
#include "ImGui/imgui.h"

#include "Editor/Command/ImGuiInputCommand.h"
#include "Math/QuatToEuler.h"
#include "Editor/Command/GroupCommand.h"
Command* Vector3Show::operator()(mtgb::Vector3* _vec, const char* _name) const
{
	mtgb::Vector3 old = *_vec;

	bool changed = ImGui::InputFloat3(_name, &_vec->x, "%.3f", ImGuiInputTextFlags_NoUndoRedo);

	if (changed == false)
		return nullptr;

	return new mtgb::ImGuiInputCommand<mtgb::Vector3>(_vec, old, *_vec, _name);
}

/// <summary>
/// <para> 四元数をオイラー角で表示する </para>
/// <para> ※オイラー角への変換は一意ではないので読み取り専用としている</para>
/// </summary>
/// <param name="vec">表示する四元数</param>
/// <param name="name">ラベル名</param>
/// <returns></returns>
Command* QuaternionShow::operator()(DirectX::XMVECTORF32* _vec, const char* _name) const
{
	mtgb::Vector3 vec3 = mtgb::QuatToEuler(*_vec);
	ImGui::InputFloat3(_name, &vec3.x, "%.3f", ImGuiInputTextFlags_ReadOnly);

	return nullptr;
}

Command* Vector4Show::operator()(DirectX::XMVECTOR* _vec, const char* _name) const
{
	DirectX::XMVECTOR old = *_vec;

	DirectX::XMFLOAT4 tmp;
	DirectX::XMStoreFloat4(&tmp, *_vec);
	bool changed = ImGui::InputFloat4(_name, &tmp.x, "%.3f", 0);

	if (changed)
	{
		// 変化があれば再度 XMVECTOR に読み込んで格納
		*_vec = DirectX::XMLoadFloat4(&tmp);
		return new mtgb::ImGuiInputCommand<DirectX::XMVECTOR>(_vec, old, *_vec, _name);
	}

	return nullptr;
}

Command* MatrixShow::operator()(DirectX::XMMATRIX* _mat, const char* _name) const
{
	Vector4Show xShow;
	Command* cmdX = xShow(&_mat->r[0], "x");

	Vector4Show yShow;
	Command* cmdY = yShow(&_mat->r[1], "y");

	Vector4Show zShow;
	Command* cmdZ = zShow(&_mat->r[2], "z");

	Vector4Show wShow;
	Command* cmdW = wShow(&_mat->r[3], "w");

	mtgb::GroupCommand* pGrpCmd = new mtgb::GroupCommand();
	if (cmdX != nullptr)
	{
		pGrpCmd->ExecuteCommand(cmdX);
	}
	if (cmdY != nullptr)
	{
		pGrpCmd->ExecuteCommand(cmdY);
	}
	if (cmdZ != nullptr)
	{
		pGrpCmd->ExecuteCommand(cmdZ);
	}
	if (cmdW != nullptr)
	{
		pGrpCmd->ExecuteCommand(cmdW);
	}

	return pGrpCmd;
}