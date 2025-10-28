#include "ShowAttributes.h"
#include "ImGuiInputCommand.h"
Command* Vector3Show::operator()(mtgb::Vector3* vec, const char* name) const
{
	mtgb::Vector3& old = *vec;

	bool changed = ImGui::InputFloat3(name, &vec->x,"%.3f",ImGuiInputTextFlags_NoUndoRedo);

	if (changed == false)
		return nullptr;
	
	return new ImGuiInputCommand<mtgb::Vector3>(vec, old, *vec, name);
}

/// <summary>
/// <para> 四元数をオイラー角で表示する </para>
/// <para> ※オイラー角への変換は一意ではないので読み取り専用としている</para>
/// </summary>
/// <param name="vec">表示する四元数</param>
/// <param name="name">ラベル名</param>
/// <returns></returns>
Command* QuaternionSHow::operator()(DirectX::XMVECTORF32* vec, const char* name) const
{
	mtgb::Vector3 vec3 = mtgb::QuatToEuler(*vec);
	ImGui::InputFloat3(name, &vec3.x,"%.3f",ImGuiInputTextFlags_ReadOnly);

	return nullptr;
}

Command* Vector4Show::operator()(DirectX::XMVECTOR* _vec, const char* _name) const
{
	DirectX::XMVECTOR& old = *_vec;

	bool changed = ImGui::InputFloat4(_name, _vec->m128_f32);;

	if (changed == false)
		return nullptr;

	return new ImGuiInputCommand<DirectX::XMVECTOR>(_vec, old, *_vec,_name);
}
