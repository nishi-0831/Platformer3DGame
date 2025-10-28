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
/// <para> �l�������I�C���[�p�ŕ\������ </para>
/// <para> ���I�C���[�p�ւ̕ϊ��͈�ӂł͂Ȃ��̂œǂݎ���p�Ƃ��Ă���</para>
/// </summary>
/// <param name="vec">�\������l����</param>
/// <param name="name">���x����</param>
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
